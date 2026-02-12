package li.cil.ocf.api.network;

import net.minecraft.nbt.NbtCompound;

import java.util.Set;
import java.util.UUID;

/**
 * Core node in OC-like computer/component network.
 */
public interface Node {
    UUID id();

    String componentName();

    Set<UUID> neighbors();

    void connect(Node other);

    void disconnect(Node other);

    NbtCompound save();

    void load(NbtCompound tag);
}
