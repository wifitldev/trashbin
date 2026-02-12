package li.cil.ocf.common.machine;

/**
 * Serializable runtime event queued for machine processing.
 */
public record MachineEvent(
        String name,
        String localAddress,
        String remoteAddress,
        int port,
        double distance,
        String payload
) {
}
