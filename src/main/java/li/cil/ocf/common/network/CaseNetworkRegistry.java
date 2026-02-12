package li.cil.ocf.common.network;

import li.cil.ocf.common.block.entity.CaseBlockEntity;
import net.minecraft.server.world.ServerWorld;
import net.minecraft.util.math.BlockPos;

import java.util.Collection;
import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;

/**
 * Server-side registry of loaded case nodes per world. Keeps modem routing deterministic.
 */
public final class CaseNetworkRegistry {
    private static final Map<ServerWorld, Map<BlockPos, CaseBlockEntity>> WORLD_CASES = new ConcurrentHashMap<>();

    private CaseNetworkRegistry() {
    }

    public static void add(ServerWorld world, CaseBlockEntity blockEntity) {
        WORLD_CASES.computeIfAbsent(world, ignored -> new ConcurrentHashMap<>())
                .put(blockEntity.getPos().toImmutable(), blockEntity);
    }

    public static void remove(ServerWorld world, BlockPos pos) {
        Map<BlockPos, CaseBlockEntity> entries = WORLD_CASES.get(world);
        if (entries != null) {
            entries.remove(pos);
            if (entries.isEmpty()) {
                WORLD_CASES.remove(world);
            }
        }
    }

    public static Collection<CaseBlockEntity> all(ServerWorld world) {
        return WORLD_CASES.getOrDefault(world, Map.of()).values();
    }
}
