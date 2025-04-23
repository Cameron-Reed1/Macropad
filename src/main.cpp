#include "states/MenuState.h"
#include "RippleAnim.h"
#include "Macropad.h"
#include "Config.h"
#include <json.h>


int main(void)
{
    Macropad& macropad = Macropad::get_instance();
    MenuState menuState = MenuState();
    CFG* cfg = Config::read();

    macropad.init(true, true, true, false, false, true, false);
    menuState.Activate();
    ripple_anim_setup();
    ripple_anim_set_brightness(cfg->brightness);

    JSONObject* obj = parseString("{\"test\": 4123}");
    JSONValue* val = nullptr;
    if (obj != nullptr) {
        val = obj->get("test");
        if (val != nullptr) {
            if (val->type == JSONType::INT) {
                menuState.val = ((JSONInt*)val)->value;
            }
        }
        delete obj;
    }

    while (true) {
        macropad.run();
    }

    return 0;
}

