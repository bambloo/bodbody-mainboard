#include "bodbody-helper.h"
#include "gui/common/FrontendApplication.hpp"
#include "gui/containers/UserInfoListItem.hpp"
#include "gui_generated/common/FrontendApplicationBase.hpp"
#include "sqlite3.h"
#include "texts/TextKeysAndLanguages.hpp"
#include "touchgfx/Screen.hpp"
#include "touchgfx/TypedText.hpp"
#include "touchgfx/Unicode.hpp"
#include "touchgfx/events/ClickEvent.hpp"
#include "touchgfx/widgets/ButtonWithIcon.hpp"
#include "utils.h"
#include <gui/prepare_screen/PrepareView.hpp>
#include <string.h>

PrepareView::PrepareView()
    : btnClickedCallback(this, &PrepareView::btnClicked),
      textClickedCallback(this, &PrepareView::textClicked),
      genderClickedCallback(this, &PrepareView::genderClicked),
      userSelectedCallback(this, &PrepareView::userSelected), selectedUser(-1) {
  buttonQuery.setClickAction(btnClickedCallback);
  buttonAdd.setClickAction(btnClickedCallback);
  buttonMod.setClickAction(btnClickedCallback);
  buttonDel.setClickAction(btnClickedCallback);
  buttonDone.setClickAction(btnClickedCallback);
  buttonCancel.setClickAction(btnClickedCallback);
  buttonNext.setClickAction(btnClickedCallback);
  buttonPrev.setClickAction(btnClickedCallback);

  buttonDelOk.setClickAction(btnClickedCallback);
  buttonDelCancel.setClickAction(btnClickedCallback);

  userArea.setClickAction(textClickedCallback);
  ageArea.setClickAction(textClickedCallback);
  heightArea.setClickAction(textClickedCallback);
  queryStringArea.setClickAction(textClickedCallback);

  genderButton.setClickAction(genderClickedCallback);
}

void PrepareView::setupScreen() {
  PrepareViewBase::setupScreen();
  FrontendApplication::getInstance()->attachKeyboardToCurrentScreen();
  FrontendApplication::getInstance()->hideKeyboard();

  profile.age = 25;
  profile.gender = 'M';
  profile.height = 180;
  profile.weight = 70;
  profile.name[0] = 0;

  updateProfile(0);

  updateUserInfoList();
  userInfoList.setItemSelectedCallback(userSelectedCallback);
}

void PrepareView::tearDownScreen() { PrepareViewBase::tearDownScreen(); }


void PrepareView::updateProfile(uint8_t mode) {
  if (mode == 0) {
    Unicode::fromUTF8((const uint8_t *)profile.name, userAreaBuffer,
                      USERAREA_SIZE);
    Unicode::snprintf(ageAreaBuffer, AGEAREA_SIZE, "%d", profile.age);
    Unicode::snprintf(heightAreaBuffer, HEIGHTAREA_SIZE, "%d", profile.height);
  }

  if (mode == 1) {
    Unicode::toUTF8(userAreaBuffer, (uint8_t *)profile.name, USERAREA_SIZE);
    profile.age = Unicode::atoi(ageAreaBuffer);
    profile.height = Unicode::atoi(heightAreaBuffer);
    if (profileMode == 1) {
      profile.weight = 0;
    }
  }
}

void PrepareView::showDeleteModal(bool show) {
  buttonDelOk.setVisible(show);
  buttonDelCancel.setVisible(show);
  buttonDelOk.invalidate();
  buttonDelCancel.invalidate();
}

void PrepareView::btnClicked(const ButtonWithIcon &button,
                             const ClickEvent &event) {
  if (event.getType() != ClickEvent::RELEASED) {
    return;
  }

  if (&button == &buttonAdd) {
    profileMode = 1;
    profile.id = 0;
    memset(&profile, 0, sizeof(profile));
    updateProfile(0);
    currMenuLabel.setTypedText(TypedText(T_ADDUSERLABEL));
    addModUserModal.setVisible(true);
    addModUserModal.invalidate();
    return;
  }

  if (&button == &buttonMod) {
    if (selectedUser < 0) {
      return;
    }
    profileMode = 2;
    profile = userInfos[selectedUser];
    updateProfile(0);
    currMenuLabel.setTypedText(TypedText(T_EDITUSERLABEL));
    addModUserModal.setVisible(true);
    addModUserModal.invalidate();
    return;
  }

  if (&button == &buttonDel) {
    if (selectedUser < 0) {
      return;
    }
    showDeleteModal(!buttonDelOk.isVisible());
    return;
  }

  if (&button == &buttonDelCancel) {
    showDeleteModal(false);
    return;
  }

  if (&button == &buttonDelOk) {
    int res = bodbody_delete_user(FrontendApplication::getDatabase(),
                                  userInfos[selectedUser].id);
    if (res == SQLITE_DONE) {
      updateUserInfoList();
    }
    showDeleteModal(false);
    invalidate();
    return;
  }

  if (&button == &buttonCancel) {
    profileMode = 0;
    addModUserModal.setVisible(false);
    currMenuLabel.setTypedText(TypedText(T_CHOOSEUSERLABEL));
    invalidate();
    return;
  }

  if (&button == &buttonDone) {
    updateProfile(1);
    uint8_t res =
        bodbody_save_user_info(FrontendApplication::getDatabase(), &profile);
    if (res == SQLITE_DONE) {
      addModUserModal.setVisible(false);
      updateUserInfoList();
    }
    invalidate();
    return;
  }

  if (&button == &buttonQuery) {
    updateUserInfoList();
    return;
  }

  if (&button == &buttonNext) {
    if (currentPage < totalPage) {
      switchPage(currentPage + 1);
    }
    return;
  }

  if (&button == &buttonPrev) {
    if (currentPage > 1) {
      switchPage(currentPage - 1);
    }
    return;
  }
}

void PrepareView::textClicked(const TextAreaWithOneWildcard &textArea,
                              const ClickEvent &event) {
  if (event.getType() != ClickEvent::RELEASED) {
    return;
  }

  if (&textArea == &userArea) {
    // if (profileMode != 1) {
    //   return;
    // }

    FrontendApplication::getInstance()->bindKeyboardTextArea(userArea,
                                                             USERAREA_SIZE);
    FrontendApplication::getInstance()->showKeyboard(0);
    return;
  }

  if (&textArea == &ageArea) {
    FrontendApplication::getInstance()->bindKeyboardTextArea(ageArea,
                                                             AGEAREA_SIZE);
    FrontendApplication::getInstance()->showKeyboard(2);
    return;
  }

  if (&textArea == &heightArea) {
    FrontendApplication::getInstance()->bindKeyboardTextArea(heightArea,
                                                             HEIGHTAREA_SIZE);
    FrontendApplication::getInstance()->showKeyboard(2);
    return;
  }

  if (&textArea == &queryStringArea) {
    FrontendApplication::getInstance()->bindKeyboardTextArea(
        queryStringArea, QUERYSTRINGAREA_SIZE);
    FrontendApplication::getInstance()->showKeyboard(0);
    return;
  }
}

void PrepareView::genderClicked(const ButtonWithLabel &button,
                                const ClickEvent &event) {
  if (event.getType() != ClickEvent::RELEASED) {
    return;
  }

  if (profile.gender == 'M') {
    profile.gender = 'F';
    genderButton.setLabelText(TypedText(T_FEMALE));
    genderButton.invalidate();
  } else {
    profile.gender = 'M';
    genderButton.setLabelText(TypedText(T_MALE));
    genderButton.invalidate();
  }
}

void PrepareView::updateUserInfoList() {
  char name[18];
  int count = Unicode::toUTF8(queryStringAreaBuffer, (uint8_t *)name,
                              QUERYSTRINGAREA_SIZE) -
              1;
  name[count] = '%';
  name[++count] = 0;

  count = bodbody_count(FrontendApplication::getDatabase(), (char *)name);
  currentPage = count ? 1 : 0;
  totalPage = (count + 6) / 7;
  Unicode::snprintf(pageIndexAreaBuffer, PAGEINDEXAREA_SIZE, "%d/%d",
                    count ? 1 : 0, totalPage);
  pageIndexArea.invalidate();
  if (!currentPage) {
    userInfoList.setNumberOfItems(0);
    userInfoList.invalidate();
  } else {
    switchPage(1);
  }
}

void PrepareView::switchPage(uint8_t page) {
  if (page > totalPage || page < 1) {
    return;
  }

  currentPage = page;

  char name[18];
  int count = Unicode::toUTF8(queryStringAreaBuffer, (uint8_t *)name,
                              QUERYSTRINGAREA_SIZE) -
              1;
  name[count] = '%';
  name[++count] = 0;

  Unicode::snprintf(pageIndexAreaBuffer, PAGEINDEXAREA_SIZE, "%d/%d",
                    count ? 1 : 0, totalPage);
  pageIndexArea.invalidate();

  count = bodbody_load_user_info(FrontendApplication::getDatabase(), name,
                                 userInfos,
                                 (page - 1) * PREPARE_VIEW_MAX_ITEM_PER_PAGE,
                                 PREPARE_VIEW_MAX_ITEM_PER_PAGE);
  userInfoList.setNumberOfItems(count);
  for (int i = 0; i < count; i++) {
    userInfoList.itemChanged(i);
    userInfoListListItems[i].setSelected(false);
  }
  selectedUser = -1;
  userInfoList.invalidate();
}

// __attribute__((used))
void PrepareView::userInfoListUpdateItem(UserInfoListItem &item,
                                         int16_t itemIndex) {
  item.setUserInfo(&userInfos[itemIndex]);
}

void PrepareView::userSelected(int16_t item) {
  if (selectedUser == item) {
    userInfoListListItems[item].setSelected(false);
    selectedUser = -1;
  } else {
    if (selectedUser >= 0) {
      userInfoListListItems[selectedUser].setSelected(false);
    }
    userInfoListListItems[item].setSelected(true);
    selectedUser = item;
  }
}

void PrepareView::handleClickEvent(const ClickEvent &event) {
  Screen::handleClickEvent(event);
  if (buttonDelOk.isVisible()) {
    if (!buttonDelOk.getAbsoluteRect().intersect(event.getX(), event.getY()) &&
        !buttonDelCancel.getAbsoluteRect().intersect(event.getX(),
                                                     event.getY()) &&
        !buttonDel.getAbsoluteRect().intersect(event.getX(), event.getY())) {
      showDeleteModal(false);
    }
  }
}