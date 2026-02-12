package li.cil.ocf;

import li.cil.ocf.common.registry.OCFBlockEntityTypes;
import li.cil.ocf.common.registry.OCFBlocks;
import net.fabricmc.api.ModInitializer;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

public final class OpenComputersFabric implements ModInitializer {
    public static final String MOD_ID = "opencomputers-fabric";
    public static final Logger LOGGER = LoggerFactory.getLogger("OpenComputers-Fabric");

    @Override
    public void onInitialize() {
        OCFBlocks.bootstrap();
        OCFBlockEntityTypes.bootstrap();
        LOGGER.info("OpenComputers Fabric bootstrap initialized.");
    }
}
