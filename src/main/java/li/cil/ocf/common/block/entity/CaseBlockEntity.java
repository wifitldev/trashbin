package li.cil.ocf.common.block.entity;

import li.cil.ocf.OpenComputersFabric;
import li.cil.ocf.api.network.Node;
import li.cil.ocf.common.component.ModemComponent;
import li.cil.ocf.common.machine.MachineEvent;
import li.cil.ocf.common.machine.MachineState;
import li.cil.ocf.common.network.CaseNetworkRegistry;
import li.cil.ocf.common.registry.OCFBlockEntityTypes;
import net.minecraft.block.BlockState;
import net.minecraft.block.entity.BlockEntity;
import net.minecraft.nbt.NbtCompound;
import net.minecraft.nbt.NbtIntArray;
import net.minecraft.registry.RegistryWrapper;
import net.minecraft.server.world.ServerWorld;
import net.minecraft.util.math.BlockPos;

import java.util.ArrayDeque;
import java.util.HashSet;
import java.util.Queue;
import java.util.Set;
import java.util.UUID;

public final class CaseBlockEntity extends BlockEntity implements Node {
    private static final int MAX_SIGNALS_PER_TICK = 8;

    private UUID nodeId = UUID.randomUUID();
    private final Set<UUID> neighbors = new HashSet<>();

    private final Queue<MachineEvent> eventQueue = new ArrayDeque<>();
    private MachineState machineState = MachineState.RUNNING;

    private final ModemComponent modem = new ModemComponent(this);
    private long ticks;

    public CaseBlockEntity(BlockPos pos, BlockState state) {
        super(OCFBlockEntityTypes.CASE, pos, state);
    }

    public void serverTick() {
        ticks++;
        if (machineState != MachineState.RUNNING) {
            return;
        }

        int processed = 0;
        while (processed < MAX_SIGNALS_PER_TICK && !eventQueue.isEmpty()) {
            MachineEvent event = eventQueue.poll();
            processed++;
            OpenComputersFabric.LOGGER.info(
                    "[OCF Machine {}] signal={} local={} remote={} port={} distance={} payload='{}'",
                    address(),
                    event.name(),
                    event.localAddress(),
                    event.remoteAddress(),
                    event.port(),
                    String.format("%.2f", event.distance()),
                    event.payload()
            );
        }
    }

    public String address() {
        return nodeId.toString().replace("-", "");
    }

    public ModemComponent modem() {
        return modem;
    }

    public MachineState machineState() {
        return machineState;
    }

    public void enqueueModemMessage(String remoteAddress, int port, double distance, String payload) {
        eventQueue.add(new MachineEvent(
                "modem_message",
                address(),
                remoteAddress,
                port,
                distance,
                payload
        ));
    }

    @Override
    public UUID id() {
        return nodeId;
    }

    @Override
    public String componentName() {
        return "computer";
    }

    @Override
    public Set<UUID> neighbors() {
        return Set.copyOf(neighbors);
    }

    @Override
    public void connect(Node other) {
        neighbors.add(other.id());
        markDirty();
    }

    @Override
    public void disconnect(Node other) {
        neighbors.remove(other.id());
        markDirty();
    }

    @Override
    public NbtCompound save() {
        NbtCompound tag = new NbtCompound();
        tag.putUuid("NodeId", nodeId);
        tag.putLong("Ticks", ticks);
        tag.putString("MachineState", machineState.name());
        tag.put("OpenPorts", new NbtIntArray(modem.openPorts().stream().mapToInt(Integer::intValue).toArray()));
        tag.putInt("Range", modem.range());
        return tag;
    }

    @Override
    public void load(NbtCompound tag) {
        if (tag.containsUuid("NodeId")) {
            nodeId = tag.getUuid("NodeId");
        }
        ticks = tag.getLong("Ticks", 0L);

        String stateName = tag.getString("MachineState", MachineState.RUNNING.name());
        try {
            machineState = MachineState.valueOf(stateName);
        } catch (IllegalArgumentException ignored) {
            machineState = MachineState.CRASHED;
        }

        if (tag.contains("OpenPorts")) {
            modem.loadPorts(tag.getIntArray("OpenPorts"));
        }
        modem.setRange(tag.getInt("Range", 32));
    }

    @Override
    protected void writeNbt(NbtCompound nbt, RegistryWrapper.WrapperLookup registries) {
        super.writeNbt(nbt, registries);
        nbt.put("Node", save());
    }

    @Override
    protected void readNbt(NbtCompound nbt, RegistryWrapper.WrapperLookup registries) {
        super.readNbt(nbt, registries);
        if (nbt.contains("Node")) {
            load(nbt.getCompound("Node"));
        }
    }

    @Override
    public void markRemoved() {
        if (world instanceof ServerWorld serverWorld) {
            CaseNetworkRegistry.remove(serverWorld, pos);
        }
        super.markRemoved();
    }

    @Override
    public void cancelRemoval() {
        super.cancelRemoval();
        if (world instanceof ServerWorld serverWorld) {
            CaseNetworkRegistry.add(serverWorld, this);
        }
    }
}
