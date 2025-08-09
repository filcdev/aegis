#include "dl_rdm6300.h"
#include "dl_board_config.h"
#include "dl_logger.h"

static Logger logger("CARD");

RDM6300Handler rdm6300Handler;

void RDM6300Handler::begin() {
    Serial2.begin(9600, SERIAL_8N1, DL_RDM6300_RX_PIN, DL_RDM6300_TX_PIN);
    rdm.begin(&Serial2);
    logger.info("RDM6300 handler started");
}

String RDM6300Handler::readTag() {
    if (rdm.get_new_tag_id()) {
        return String(rdm.get_tag_id(), HEX);
    }
    return "";
}
