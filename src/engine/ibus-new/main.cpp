#include <QDebug>
#include "riti.h"
#include "Settings.h"

void update_with_settings() {
    qputenv("RITI_LAYOUT_FILE", gSettings->getLayoutPath().toLatin1());
}

int main(int argc, char* argv[]) {
    gSettings = new Settings();
    update_with_settings();
    RitiContext *ctx = riti_context_new();
    qDebug() << riti_context_key_handled(ctx);
    riti_context_free(ctx);
    return 0;
}
