package MedReminder;

public class ReminderRecord
{
  private boolean exists;
  private boolean retryNeeded;
  
  private userInfo user;
  
  private Reminder[] reminders;
    
  public ReminderRecord()
  {
  }

  public ReminderRecord(userInfo newUser)
  {
    exists = true;
    retryNeeded = false;
    user = newUser;
  }

  public void setExists(boolean exists)
  {
    this.exists = exists;
  }

  public boolean isExists()
  {
    return exists;
  }

  public void setRetryNeeded(boolean retryNeeded)
  {
    this.retryNeeded = retryNeeded;
  }

  public boolean isRetryNeeded()
  {
    return retryNeeded;
  }

  public void setReminders(Reminder[] reminders)
  {
    this.reminders = reminders;
  }

  public Reminder[] getReminders()
  {
    return reminders;
  }
}
