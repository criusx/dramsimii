package MedReminder;

public class Reminder
{
  private String medicationName;
  private String ID;
  private int reminderPeriod;
  private long lastSent;
  private int dose;

  public Reminder()
  {
  }

  public Reminder(String medicationName, String ID, int reminderPeriod, 
                  long lastSent, int dose)
  {
    this.medicationName = medicationName;
    this.ID = ID;
    this.reminderPeriod = reminderPeriod;
    this.lastSent = lastSent;
    this.dose = dose;
  }

  public void setMedicationName(String medicationName)
  {
    this.medicationName = medicationName;
  }

  public String getMedicationName()
  {
    return medicationName;
  }

  public void setID(String iD)
  {
    this.ID = iD;
  }

  public String getID()
  {
    return ID;
  }

  public void setReminderPeriod(int reminderPeriod)
  {
    this.reminderPeriod = reminderPeriod;
  }

  public int getReminderPeriod()
  {
    return reminderPeriod;
  }

  public void setLastSent(long lastSent)
  {
    this.lastSent = lastSent;
  }

  public long getLastSent()
  {
    return lastSent;
  }

  public void setDose(int dose)
  {
    this.dose = dose;
  }

  public int getDose()
  {
    return dose;
  }
}
