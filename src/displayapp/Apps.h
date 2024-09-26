#pragma once

namespace Pinetime {
  namespace Applications {
    enum class Apps {
      None,
      Launcher,
      Clock,
      SysInfo,
      FirmwareUpdate,
      FirmwareValidation,
      NotificationsPreview,
      Notifications,
      Timer,
      Alarm,
      FlashLight,
      BatteryInfo,
#ifdef MUS
      Music,
#endif
      Paint,
      Paddle,
      Twos,
      HeartRate,
#ifdef NAV
      Navigation,
#endif
      StopWatch,
      Metronome,
      Motion,
      Steps,
#ifdef WEATH
      Weather,
#endif
      PassKey,
      QuickSettings,
      Settings,
      SettingWatchFace,
      SettingTimeFormat,
      SettingDisplay,
      SettingWakeUp,
      SettingSteps,
      SettingSetDate,
      SettingSetTime,
      SettingChimes,
      SettingShakeThreshold,
      SettingBluetooth,
      Error
    };
  }
}
