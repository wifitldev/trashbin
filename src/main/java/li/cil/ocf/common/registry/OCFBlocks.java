package li.cil.ocf.common.registry;

import li.cil.ocf.OpenComputersFabric;
import li.cil.ocf.common.block.CaseBlock;
import net.minecraft.block.AbstractBlock;
import net.minecraft.block.Block;
import net.minecraft.block.MapColor;
import net.minecraft.item.BlockItem;
import net.minecraft.item.Item;
import net.minecraft.registry.Registries;
import net.minecraft.registry.Registry;
import net.minecraft.util.Identifier;

public final class OCFBlocks {
    public static final Block CASE = register("case",
            new CaseBlock(AbstractBlock.Settings.create().strength(2.0F).mapColor(MapColor.IRON_GRAY)));

    private OCFBlocks() {
    }

    public static void bootstrap() {
        // no-op
    }

    private static Block register(String path, Block block) {
        Identifier id = Identifier.of(OpenComputersFabric.MOD_ID, path);
        Registry.register(Registries.BLOCK, id, block);
        Registry.register(Registries.ITEM, id, new BlockItem(block, new Item.Settings()));
        return block;
    }
}
