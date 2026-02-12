package li.cil.ocf.common.command;

import com.mojang.brigadier.CommandDispatcher;
import com.mojang.brigadier.arguments.IntegerArgumentType;
import com.mojang.brigadier.arguments.StringArgumentType;
import com.mojang.brigadier.context.CommandContext;
import com.mojang.brigadier.exceptions.CommandSyntaxException;
import com.mojang.brigadier.exceptions.SimpleCommandExceptionType;
import li.cil.ocf.common.block.entity.CaseBlockEntity;
import net.fabricmc.fabric.api.command.v2.CommandRegistrationCallback;
import net.minecraft.command.CommandRegistryAccess;
import net.minecraft.server.command.CommandManager;
import net.minecraft.server.command.ServerCommandSource;
import net.minecraft.server.world.ServerWorld;
import net.minecraft.text.Text;
import net.minecraft.util.math.BlockPos;

import static net.minecraft.server.command.CommandManager.argument;
import static net.minecraft.server.command.CommandManager.literal;

public final class OCFCommands {
    private static final SimpleCommandExceptionType CASE_NOT_FOUND =
            new SimpleCommandExceptionType(Text.literal("No CaseBlockEntity at provided coordinates"));

    private OCFCommands() {
    }

    public static void bootstrap() {
        CommandRegistrationCallback.EVENT.register(OCFCommands::register);
    }

    private static void register(CommandDispatcher<ServerCommandSource> dispatcher,
                                 CommandRegistryAccess registryAccess,
                                 CommandManager.RegistrationEnvironment environment) {
        dispatcher.register(literal("oc_addr")
                .then(argument("x", IntegerArgumentType.integer())
                        .then(argument("y", IntegerArgumentType.integer())
                                .then(argument("z", IntegerArgumentType.integer())
                                        .executes(ctx -> {
                                            CaseBlockEntity caseEntity = requireCase(ctx);
                                            ctx.getSource().sendFeedback(() -> Text.literal(caseEntity.address()), false);
                                            return 1;
                                        }))));

        dispatcher.register(literal("oc_open")
                .then(argument("x", IntegerArgumentType.integer())
                        .then(argument("y", IntegerArgumentType.integer())
                                .then(argument("z", IntegerArgumentType.integer())
                                        .then(argument("port", IntegerArgumentType.integer(1, 65535))
                                                .executes(ctx -> {
                                                    CaseBlockEntity caseEntity = requireCase(ctx);
                                                    int port = IntegerArgumentType.getInteger(ctx, "port");
                                                    boolean changed = caseEntity.modem().open(port);
                                                    ctx.getSource().sendFeedback(() -> Text.literal("open(" + port + ") => " + changed), false);
                                                    return changed ? 1 : 0;
                                                }))))));

        dispatcher.register(literal("oc_broadcast")
                .then(argument("x", IntegerArgumentType.integer())
                        .then(argument("y", IntegerArgumentType.integer())
                                .then(argument("z", IntegerArgumentType.integer())
                                        .then(argument("port", IntegerArgumentType.integer(1, 65535))
                                                .then(argument("msg", StringArgumentType.greedyString())
                                                        .executes(ctx -> {
                                                            CaseBlockEntity caseEntity = requireCase(ctx);
                                                            int port = IntegerArgumentType.getInteger(ctx, "port");
                                                            String msg = StringArgumentType.getString(ctx, "msg");
                                                            caseEntity.modem().broadcast(port, msg);
                                                            ctx.getSource().sendFeedback(() -> Text.literal("broadcast sent on port " + port), false);
                                                            return 1;
                                                        }))))));

        dispatcher.register(literal("oc_send")
                .then(argument("x", IntegerArgumentType.integer())
                        .then(argument("y", IntegerArgumentType.integer())
                                .then(argument("z", IntegerArgumentType.integer())
                                        .then(argument("toAddress", StringArgumentType.word())
                                                .then(argument("port", IntegerArgumentType.integer(1, 65535))
                                                        .then(argument("msg", StringArgumentType.greedyString())
                                                                .executes(ctx -> {
                                                                    CaseBlockEntity caseEntity = requireCase(ctx);
                                                                    String toAddress = StringArgumentType.getString(ctx, "toAddress");
                                                                    int port = IntegerArgumentType.getInteger(ctx, "port");
                                                                    String msg = StringArgumentType.getString(ctx, "msg");
                                                                    caseEntity.modem().send(toAddress, port, msg);
                                                                    ctx.getSource().sendFeedback(() -> Text.literal("send issued to " + toAddress), false);
                                                                    return 1;
                                                                })))))));
    }

    private static CaseBlockEntity requireCase(CommandContext<ServerCommandSource> ctx) throws CommandSyntaxException {
        int x = IntegerArgumentType.getInteger(ctx, "x");
        int y = IntegerArgumentType.getInteger(ctx, "y");
        int z = IntegerArgumentType.getInteger(ctx, "z");
        BlockPos pos = new BlockPos(x, y, z);

        ServerWorld world = ctx.getSource().getWorld();
        if (world.getBlockEntity(pos) instanceof CaseBlockEntity caseEntity) {
            return caseEntity;
        }
        throw CASE_NOT_FOUND.create();
    }
}
