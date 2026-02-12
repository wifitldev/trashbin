package li.cil.ocf.common.registry;

import li.cil.ocf.OpenComputersFabric;
import li.cil.ocf.common.block.entity.CaseBlockEntity;
import net.fabricmc.fabric.api.object.builder.v1.block.entity.FabricBlockEntityTypeBuilder;
import net.minecraft.block.entity.BlockEntityType;
import net.minecraft.registry.Registries;
import net.minecraft.registry.Registry;
import net.minecraft.util.Identifier;

public final class OCFBlockEntityTypes {
    public static final BlockEntityType<CaseBlockEntity> CASE = Registry.register(
            Registries.BLOCK_ENTITY_TYPE,
            Identifier.of(OpenComputersFabric.MOD_ID, "case"),
            FabricBlockEntityTypeBuilder.create(CaseBlockEntity::new, OCFBlocks.CASE).build()
    );

    private OCFBlockEntityTypes() {
    }

    public static void bootstrap() {
        // no-op
    }
}
