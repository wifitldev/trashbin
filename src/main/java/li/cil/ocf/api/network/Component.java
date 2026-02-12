package li.cil.ocf.api.network;

import java.util.Set;

/**
 * Minimal addressable component contract for OC-like component bus semantics.
 */
public interface Component {
    String address();

    String type();

    Set<String> methods();
}
