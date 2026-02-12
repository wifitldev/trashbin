package li.cil.ocf.common.block.entity;

import li.cil.ocf.api.network.Node;
import li.cil.ocf.common.registry.OCFBlockEntityTypes;
import net.minecraft.block.BlockState;
import net.minecraft.block.entity.BlockEntity;
import net.minecraft.nbt.NbtCompound;
import net.minecraft.registry.RegistryWrapper;
import net.minecraft.util.math.BlockPos;

import java.util.HashSet;
import java.util.Set;
import java.util.UUID;

public final class CaseBlockEntity extends BlockEntity implements Node {
    private final UUID nodeId = UUID.randomUUID();
    private final Set<UUID> neighbors = new HashSet<>();
    private long ticks;

    public CaseBlockEntity(BlockPos pos, BlockState state) {
        super(OCFBlockEntityTypes.CASE, pos, state);
    }

    public void serverTick() {
        ticks++;
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
        return tag;
    }

    @Override
    public void load(NbtCompound tag) {
        ticks = tag.getLong("Ticks", 0L);
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
}
