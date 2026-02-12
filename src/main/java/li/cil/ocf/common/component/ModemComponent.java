package li.cil.ocf.common.component;

import li.cil.ocf.OpenComputersFabric;
import li.cil.ocf.api.network.Component;
import li.cil.ocf.common.block.entity.CaseBlockEntity;
import li.cil.ocf.common.network.CaseNetworkRegistry;
import net.minecraft.server.world.ServerWorld;

import java.util.HashSet;
import java.util.Set;

/**
 * Simplified OC-like wireless modem implementation.
 */
public final class ModemComponent implements Component {
    private static final int MIN_PORT = 1;
    private static final int MAX_PORT = 65_535;
    private static final int MAX_DELIVERIES_PER_TICK = 20;
    private static final int DEFAULT_RANGE = 32;

    private final CaseBlockEntity owner;
    private final Set<Integer> openPorts = new HashSet<>();

    private int range = DEFAULT_RANGE;
    private long budgetTick = Long.MIN_VALUE;
    private int deliveriesThisTick = 0;

    public ModemComponent(CaseBlockEntity owner) {
        this.owner = owner;
    }

    @Override
    public String address() {
        return owner.address();
    }

    @Override
    public String type() {
        return "modem";
    }

    @Override
    public Set<String> methods() {
        return Set.of("open", "close", "closeAll", "isOpen", "broadcast", "send");
    }

    public boolean open(int port) {
        if (!isValidPort(port)) {
            return false;
        }
        boolean changed = openPorts.add(port);
        if (changed) {
            owner.markDirty();
        }
        return changed;
    }

    public boolean close(int port) {
        boolean changed = openPorts.remove(port);
        if (changed) {
            owner.markDirty();
        }
        return changed;
    }

    public void closeAll() {
        if (!openPorts.isEmpty()) {
            openPorts.clear();
            owner.markDirty();
        }
    }

    public boolean isOpen(int port) {
        return openPorts.contains(port);
    }

    public void setRange(int range) {
        this.range = Math.max(1, range);
    }

    public int range() {
        return range;
    }

    public Set<Integer> openPorts() {
        return Set.copyOf(openPorts);
    }

    public void loadPorts(int[] ports) {
        openPorts.clear();
        for (int port : ports) {
            if (isValidPort(port)) {
                openPorts.add(port);
            }
        }
    }

    public void broadcast(int port, String rawPayload) {
        if (!isValidPort(port) || owner.getWorld() == null || owner.getWorld().isClient()) {
            return;
        }

        String payload = sanitizePayload(rawPayload);
        if (payload == null) {
            return;
        }

        ServerWorld world = (ServerWorld) owner.getWorld();
        for (CaseBlockEntity target : CaseNetworkRegistry.all(world)) {
            if (target == owner) {
                continue;
            }
            deliverTo(world, target, port, payload);
        }
    }

    public void send(String toAddress, int port, String rawPayload) {
        if (!isValidPort(port) || owner.getWorld() == null || owner.getWorld().isClient()) {
            return;
        }

        String payload = sanitizePayload(rawPayload);
        if (payload == null) {
            return;
        }

        ServerWorld world = (ServerWorld) owner.getWorld();
        for (CaseBlockEntity target : CaseNetworkRegistry.all(world)) {
            if (target == owner) {
                continue;
            }
            if (target.address().equalsIgnoreCase(toAddress)) {
                deliverTo(world, target, port, payload);
                return;
            }
        }
    }

    private void deliverTo(ServerWorld world, CaseBlockEntity target, int port, String payload) {
        if (!target.modem().isOpen(port)) {
            return;
        }

        double distance = owner.getPos().toCenterPos().distanceTo(target.getPos().toCenterPos());
        if (distance > range) {
            return;
        }

        if (!consumeBudget(world.getTime())) {
            OpenComputersFabric.LOGGER.warn("Modem {} exceeded per-tick delivery budget", owner.address());
            return;
        }

        target.enqueueModemMessage(owner.address(), port, distance, payload);
    }

    private boolean consumeBudget(long worldTime) {
        if (budgetTick != worldTime) {
            budgetTick = worldTime;
            deliveriesThisTick = 0;
        }
        if (deliveriesThisTick >= MAX_DELIVERIES_PER_TICK) {
            return false;
        }
        deliveriesThisTick++;
        return true;
    }

    private static boolean isValidPort(int port) {
        return port >= MIN_PORT && port <= MAX_PORT;
    }

    private static String sanitizePayload(String payload) {
        if (payload == null || payload.isBlank()) {
            return null;
        }
        return payload.length() > 256 ? payload.substring(0, 256) : payload;
    }
}
