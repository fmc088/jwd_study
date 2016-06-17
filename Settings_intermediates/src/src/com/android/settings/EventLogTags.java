/* This file is auto-generated.  DO NOT MODIFY.
 * Source file: packages/apps/Settings/src/com/android/settings/EventLogTags.logtags
 */

package com.android.settings;

/**
 * @hide
 */
public class EventLogTags {
  private EventLogTags() { }  // don't instantiate

  /** 90200 lock_screen_type (type|3) */
  public static final int LOCK_SCREEN_TYPE = 90200;

  public static void writeLockScreenType(String type) {
    android.util.EventLog.writeEvent(LOCK_SCREEN_TYPE, type);
  }
}
