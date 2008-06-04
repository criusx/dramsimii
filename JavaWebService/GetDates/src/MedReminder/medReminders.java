package MedReminder;

public class medReminders
{
  private boolean exists;
  private boolean retryNeeded;
  private String name;
  private String email;
  private String phoneNumber;
  private String provider;
  private String[] medicationName;
  private int[] reminderFrequency;
  private int[] doses;
  private String[] lastSent;
  private String[] ID;

  public medReminders()
  {
  }

  public medReminders(userInfo newUser)
  {
    exists = true;
    retryNeeded = false;
    name = newUser.getName();
    email = newUser.getEmail();
    phoneNumber = newUser.getPhoneNumber();
    provider = newUser.getProvider();
  }

  public void setMedicationName(String[] medicationName)
  {
    this.medicationName = medicationName;
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

  public void setName(String name)
  {
    this.name = name;
  }

  public String getName()
  {
    return name;
  }

  public void setEmail(String email)
  {
    this.email = email;
  }

  public String getEmail()
  {
    return email;
  }

  public void setPhoneNumber(String phoneNumber)
  {
    this.phoneNumber = phoneNumber;
  }

  public String getPhoneNumber()
  {
    return phoneNumber;
  }

  public void setProvider(String provider)
  {
    this.provider = provider;
  }

  public String getProvider()
  {
    return provider;
  }

  public String[] getMedicationName()
  {
    return medicationName;
  }

  public void setReminderFrequency(int[] reminderFrequency)
  {
    this.reminderFrequency = reminderFrequency;
  }

  public int[] getReminderFrequency()
  {
    return reminderFrequency;
  }

  public void setDoses(int[] doses)
  {
    this.doses = doses;
  }

  public int[] getDoses()
  {
    return doses;
  }

  public void setLastSent(String[] lastSent)
  {
    this.lastSent = lastSent;
  }

  public String[] getLastSent()
  {
    return lastSent;
  }

  public void setID(String[] iD)
  {
    this.ID = iD;
  }

  public String[] getID()
  {
    return ID;
  }
}
