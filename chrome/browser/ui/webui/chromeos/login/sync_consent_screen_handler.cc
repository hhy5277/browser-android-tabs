// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/ui/webui/chromeos/login/sync_consent_screen_handler.h"

#include "base/strings/utf_string_conversions.h"
#include "chrome/browser/chromeos/login/screens/sync_consent_screen.h"
#include "chrome/grit/generated_resources.h"
#include "components/login/localized_values_builder.h"
#include "ui/base/l10n/l10n_util.h"

namespace {

const char kJsScreenPath[] = "login.SyncConsentScreen";

// This helper function gets strings from WebUI and a set of known string
// resource ids, and converts strings back to IDs. It CHECKs if string is not
// found in resources.
void GetConsentIDs(const std::unordered_set<int>& known_ids,
                   const login::StringList& consent_description,
                   const std::string& consent_confirmation,
                   std::vector<int>* consent_description_ids,
                   int* consent_confirmation_id) {
  std::unordered_map<std::string, int> known_strings;
  for (const int& id : known_ids) {
    // When the strings are passed to the HTML, the Unicode NBSP symbol
    // (\u00A0) will be automatically replaced with "&nbsp;". This change must
    // be mirrored in the string-to-ids map. Note that "\u00A0" is actually two
    // characters, so we must use base::ReplaceSubstrings* rather than
    // base::ReplaceChars.
    // TODO(alemate): Find a more elegant solution.
    std::string sanitized_string =
        base::UTF16ToUTF8(l10n_util::GetStringUTF16(id));
    base::ReplaceSubstringsAfterOffset(&sanitized_string, 0,
                                       "\u00A0" /* NBSP */, "&nbsp;");

    known_strings[sanitized_string] = id;
  }
  // The strings returned by the WebUI are not free-form, they must belong into
  // a pre-determined set of strings (stored in |string_to_grd_id_map_|). As
  // this has privacy and legal implications, CHECK the integrity of the strings
  // received from the renderer process before recording the consent.
  for (const std::string& text : consent_description) {
    auto iter = known_strings.find(text);
    CHECK(iter != known_strings.end()) << "Unexpected string:\n" << text;
    consent_description_ids->push_back(iter->second);
  }

  auto iter = known_strings.find(consent_confirmation);
  CHECK(iter != known_strings.end()) << "Unexpected string:\n"
                                     << consent_confirmation;
  *consent_confirmation_id = iter->second;
}

}  // namespace

namespace chromeos {

SyncConsentScreenHandler::SyncConsentScreenHandler()
    : BaseScreenHandler(kScreenId) {
  set_call_js_prefix(kJsScreenPath);
}

SyncConsentScreenHandler::~SyncConsentScreenHandler() {}

void SyncConsentScreenHandler::RememberLocalizedValue(
    const std::string& name,
    const int resource_id,
    ::login::LocalizedValuesBuilder* builder) {
  CHECK(known_string_ids_.count(resource_id) == 0);
  known_string_ids_.insert(resource_id);
  builder->Add(name, resource_id);
}

void SyncConsentScreenHandler::DeclareLocalizedValues(
    ::login::LocalizedValuesBuilder* builder) {
  known_string_ids_.clear();

  RememberLocalizedValue("syncConsentScreenTitle",
                         IDS_LOGIN_SYNC_CONSENT_SCREEN_TITLE, builder);
  RememberLocalizedValue("syncConsentScreenChromeSyncName",
                         IDS_LOGIN_SYNC_CONSENT_SCREEN_CHROME_SYNC_NAME,
                         builder);
  RememberLocalizedValue("syncConsentScreenChromeSyncDescription",
                         IDS_LOGIN_SYNC_CONSENT_SCREEN_CHROME_SYNC_DESCRIPTION,
                         builder);
  RememberLocalizedValue(
      "syncConsentScreenPersonalizeGoogleServicesName",
      IDS_LOGIN_SYNC_CONSENT_SCREEN_PERSONALIZE_GOOGLE_SERVICES_NAME, builder);
  RememberLocalizedValue(
      "syncConsentScreenPersonalizeGoogleServicesDescription",
      IDS_LOGIN_SYNC_CONSENT_SCREEN_PERSONALIZE_GOOGLE_SERVICES_DESCRIPTION,
      builder);
  RememberLocalizedValue(
      "syncConsentReviewSyncOptionsText",
      IDS_LOGIN_SYNC_CONSENT_SCREEN_REVIEW_SYNC_OPTIONS_LATER, builder);

  RememberLocalizedValue("syncConsentNewScreenTitle",
                         IDS_LOGIN_SYNC_CONSENT_GET_GOOGLE_SMARTS, builder);
  RememberLocalizedValue("syncConsentNewBookmarksDesc",
                         IDS_LOGIN_SYNC_CONSENT_YOUR_BOOKMARKS_ON_ALL_DEVICES,
                         builder);
  RememberLocalizedValue("syncConsentNewServicesDesc",
                         IDS_LOGIN_SYNC_CONSENT_PERSONALIZED_GOOGLE_SERVICES,
                         builder);
  RememberLocalizedValue("syncConsentNewImproveChrome",
                         IDS_LOGIN_SYNC_CONSENT_IMPROVE_CHROME, builder);
  RememberLocalizedValue("syncConsentNewGoogleMayUse",
                         IDS_LOGIN_SYNC_CONSENT_GOOGLE_MAY_USE, builder);
  RememberLocalizedValue("syncConsentNewMoreOptions",
                         IDS_LOGIN_SYNC_CONSENT_MORE_OPTIONS, builder);
  RememberLocalizedValue("syncConsentNewYesIAmIn",
                         IDS_LOGIN_SYNC_CONSENT_YES_I_AM_IN, builder);
  RememberLocalizedValue("syncConsentNewSyncOptions",
                         IDS_LOGIN_SYNC_CONSENT_SYNC_OPTIONS, builder);
  RememberLocalizedValue("syncConsentNewSyncOptionsSubtitle",
                         IDS_LOGIN_SYNC_CONSENT_SYNC_OPTIONS_SUBTITLE, builder);
  RememberLocalizedValue("syncConsentNewChooseOption",
                         IDS_LOGIN_SYNC_CONSENT_CHOOSE_OPTION, builder);
  RememberLocalizedValue("syncConsentNewOptionReview",
                         IDS_LOGIN_SYNC_CONSENT_OPTION_REVIEW, builder);
  RememberLocalizedValue("syncConsentNewOptionReviewDsc",
                         IDS_LOGIN_SYNC_CONSENT_OPTION_REVIEW_DSC, builder);
  RememberLocalizedValue("syncConsentNewOptionJustSync",
                         IDS_LOGIN_SYNC_CONSENT_OPTION_JUST_SYNC, builder);
  RememberLocalizedValue("syncConsentNewOptionJustSyncDsc",
                         IDS_LOGIN_SYNC_CONSENT_OPTION_JUST_SYNC_DSC, builder);
  RememberLocalizedValue("syncConsentNewOptionSyncAndPersonalization",
                         IDS_LOGIN_SYNC_CONSENT_OPTION_SYNC_AND_PERSONALIZATION,
                         builder);
  RememberLocalizedValue(
      "syncConsentNewOptionSyncAndPersonalizationDsc",
      IDS_LOGIN_SYNC_CONSENT_OPTION_SYNC_AND_PERSONALIZATION_DSC, builder);

  RememberLocalizedValue("syncConsentAcceptAndContinue",
                         IDS_LOGIN_SYNC_CONSENT_SCREEN_ACCEPT_AND_CONTINUE,
                         builder);
}

void SyncConsentScreenHandler::Bind(SyncConsentScreen* screen) {
  screen_ = screen;
  BaseScreenHandler::SetBaseScreen(screen);
}

void SyncConsentScreenHandler::Show() {
  ShowScreen(kScreenId);
}

void SyncConsentScreenHandler::Hide() {}

void SyncConsentScreenHandler::SetThrobberVisible(bool visible) {
  CallJS("login.SyncConsentScreen.setThrobberVisible", visible);
}

void SyncConsentScreenHandler::Initialize() {}

void SyncConsentScreenHandler::RegisterMessages() {
  AddCallback("login.SyncConsentScreen.continueAndReview",
              &SyncConsentScreenHandler::HandleContinueAndReview);
  AddCallback("login.SyncConsentScreen.continueWithDefaults",
              &SyncConsentScreenHandler::HandleContinueWithDefaults);
}

void SyncConsentScreenHandler::GetAdditionalParameters(
    base::DictionaryValue* parameters) {
  parameters->Set("syncConsentMakeBetter",
                  std::make_unique<base::Value>(false));
  BaseScreenHandler::GetAdditionalParameters(parameters);
}

void SyncConsentScreenHandler::HandleContinueAndReview(
    const login::StringList& consent_description,
    const std::string& consent_confirmation) {
  std::vector<int> consent_description_ids;
  int consent_confirmation_id;
  GetConsentIDs(known_string_ids_, consent_description, consent_confirmation,
                &consent_description_ids, &consent_confirmation_id);
  screen_->OnContinueAndReview(consent_description_ids,
                               consent_confirmation_id);

  SyncConsentScreen::SyncConsentScreenTestDelegate* test_delegate =
      screen_->GetDelegateForTesting();
  if (test_delegate) {
    test_delegate->OnConsentRecordedStrings(consent_description,
                                            consent_confirmation);
  }
}

void SyncConsentScreenHandler::HandleContinueWithDefaults(
    const login::StringList& consent_description,
    const std::string& consent_confirmation) {
  std::vector<int> consent_description_ids;
  int consent_confirmation_id;
  GetConsentIDs(known_string_ids_, consent_description, consent_confirmation,
                &consent_description_ids, &consent_confirmation_id);
  screen_->OnContinueWithDefaults(consent_description_ids,
                                  consent_confirmation_id);

  SyncConsentScreen::SyncConsentScreenTestDelegate* test_delegate =
      screen_->GetDelegateForTesting();
  if (test_delegate) {
    test_delegate->OnConsentRecordedStrings(consent_description,
                                            consent_confirmation);
  }
}

}  // namespace chromeos
