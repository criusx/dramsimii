package COREMedDemo;

public class patientRecord
{
  private String firstName;
  private String middleName;
  private String lastName;
  private long DOB;
  private String[] medications;
  private String[] allergies;
  private String RFIDnum;
  private byte[] image;
  private boolean exists;

  public patientRecord()
  {
  }
  
  public patientRecord(boolean _exists)
  {
    exists = _exists;
  }
  
  public void setFirstName(String firstName)
  {
    this.firstName = firstName;
  }

  public String getFirstName()
  {
    return firstName;
  }

  public void setMiddleName(String middleName)
  {
    this.middleName = middleName;
  }

  public String getMiddleName()
  {
    return middleName;
  }

  public void setLastName(String lastName)
  {
    this.lastName = lastName;
  }

  public String getLastName()
  {
    return lastName;
  }

  public void setDOB(long dOB)
  {
    this.DOB = dOB;
  }

  public long getDOB()
  {
    return DOB;
  }

  public void setMedications(String[] medications)
  {
    this.medications = medications;
  }

  public String[] getMedications()
  {
    return medications;
  }

  public void setAllergies(String[] allergies)
  {
    this.allergies = allergies;
  }

  public String[] getAllergies()
  {
    return allergies;
  }

  public void setRFIDnum(String rFIDnum)
  {
    this.RFIDnum = rFIDnum;
  }

  public String getRFIDnum()
  {
    return RFIDnum;
  }

  public void setImage(byte[] image)
  {
    this.image = image;
  }

  public byte[] getImage()
  {
    return image;
  }

  public void setExists(boolean exists)
  {
    this.exists = exists;
  }

  public boolean isExists()
  {
    return exists;
  }
}
