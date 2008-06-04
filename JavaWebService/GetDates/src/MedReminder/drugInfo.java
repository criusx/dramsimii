package MedReminder;

public class drugInfo
{
  private String RFIDNum;
  private String description;
  private byte[] picture;
  private int[] reminderIntervals;
  private int[] doses;
  private String name;
  private boolean authenticated;
  private boolean exists;
  private boolean retryNeeded;

  public drugInfo(boolean _exists)
  {
    exists = false;
    retryNeeded = false;
  }

  public drugInfo()
  {
    exists = false;
    retryNeeded = false;
  }

  public drugInfo(String rfidNum, boolean retry)
  {
    exists = false;
    description = "";
    RFIDNum = rfidNum;
    retryNeeded = retry;
  }

  public void setRFIDNum(String rFIDNum)
  {
    this.RFIDNum = rFIDNum;
  }

  public String getRFIDNum()
  {
    return RFIDNum;
  }

  public void setDescription(String description)
  {
    this.description = description;
  }

  public String getDescription()
  {
    return description;
  }

  public void setPicture(byte[] picture)
  {
    this.picture = picture;
  }

  public byte[] getPicture()
  {
    return picture;
  }

  public void setReminderIntervals(int[] reminderIntervals)
  {
    this.reminderIntervals = reminderIntervals;
  }

  public int[] getReminderIntervals()
  {
    return reminderIntervals;
  }

  public void setDoses(int[] doses)
  {
    this.doses = doses;
  }

  public int[] getDoses()
  {
    return doses;
  }

  public void setName(String name)
  {
    this.name = name;
  }

  public String getName()
  {
    return name;
  }

  public void setAuthenticated(boolean authenticated)
  {
    this.authenticated = authenticated;
  }

  public boolean isAuthenticated()
  {
    return authenticated;
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
}
