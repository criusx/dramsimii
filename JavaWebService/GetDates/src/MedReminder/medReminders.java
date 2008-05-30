package MedReminder;

public class medReminders
{
  boolean exists;
  boolean retryNeeded;
  String name;
  String email;
  String phoneNumber;
  String provider;
  String[] medicationName;
  String[] reminderFrequency;
  String[] doses;
  String[] lastSent;
  String[] ID;

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

  public String[] getMedicationName()
  {
    return medicationName;
  }

  public void setReminderFrequency(String[] reminderFrequency)
  {
    this.reminderFrequency = reminderFrequency;
  }

  public String[] getReminderFrequency()
  {
    return reminderFrequency;
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

 

  public void setID(String[] iD)
  {
    this.ID = iD;
  }

  public String[] getID()
  {
    return ID;
  }

  public void setDoses(String[] doses)
  {
    this.doses = doses;
  }

  public String[] getDoses()
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
}
