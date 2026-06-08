#include "hash.h"
#include "stm32_hal_legacy.h"
#include "stm32h7xx_hal_hash.h"
#include "touchgfx/Texts.hpp"
#include <gui/login_screen/LoginView.hpp>
#include <gui/login_screen/LoginPresenter.hpp>

#include "stm32h7xx_hal_hash_ex.h"
#include "parameters.h"

LoginPresenter::LoginPresenter(LoginView& v)
    : view(v)
{

}

void LoginPresenter::activate()
{
    
}

void LoginPresenter::deactivate()
{

}

bool LoginPresenter::checkPin(uint32_t pin)
{
    uint8_t hash[20];
    HAL_HASH_SHA1_Start(&hhash, (uint8_t *)&pin, 4, hash, 1000);
    if (memcmp(hash, parameters_get()->pin_sha1, 20) == 0) {
        return true;
    } else {
        return false;
    }
}