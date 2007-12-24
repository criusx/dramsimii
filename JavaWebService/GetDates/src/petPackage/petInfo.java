package petPackage;

public class petInfo
{
  private boolean exists;
  private byte[] image;
  private String rfidNum;
  private String contactInfo;
  private String owner;
  private String workPhone;
  private String homePhone;
  private String cellPhone;
  private String vetPhone;
  private String email;
  private String breed;
  private String tagCode;
  private String name;
  private String preferredFood;
  private String password;
  private String medicalNeeds;
  private String medications;
  private String vetName;
  private String vetAddress;
  private int reward;

  public petInfo(byte[] _image, String _contactInfo, String _owner, 
                 String _workPhone, String _rfidNum, String _homePhone, 
                 String _cellPhone, String _email, String _breed, 
                 String _tagCode, String _name, String _preferredFood, 
                 String _medicalNeeds, String _medications, 
                 String _vetName, String _vetAddress, String _vetPhone, 
                 int _reward)
  {
    exists = true;
    rfidNum = _rfidNum;
    owner = _owner;
    contactInfo = _contactInfo;
    breed = _breed;
    image = _image;
    tagCode = _tagCode;
    name = _name;
    homePhone = _homePhone;
    cellPhone = _cellPhone;
    workPhone = _workPhone;
    email = _email;
    preferredFood = _preferredFood;
    medicalNeeds = _medicalNeeds;
    medications = _medications;
    vetName = _vetName;
    vetPhone = _vetPhone;
    vetAddress = _vetAddress;
    reward = _reward;


  }

  public petInfo(String _rfidNum)
  {
    exists = false;
    rfidNum = _rfidNum;
  }

  public petInfo()
  {
    exists = false;
  }

  public String getContactInfo()
  {
    return contactInfo;
  }

  public void setContactInfo(String _contactInfo)
  {
    contactInfo = _contactInfo;
  }

  public boolean isExists()
  {
    return exists;
  }

  public void setExists(boolean _exists)
  {
    exists = _exists;
  }

  public byte[] getImage()
  {
    return image;
  }

  public void setImage(byte[] _image)
  {
    image = _image;
  }

  public String getRfidNum()
  {
    return rfidNum;
  }

  public void setRfidNum(String _rfidNum)
  {
    rfidNum = _rfidNum;
  }

  public String getOwner()
  {
    return owner;
  }

  public void setOwner(String _owner)
  {
    owner = _owner;
  }

  public String getBreed()
  {
    return breed;
  }

  public void setBreed(String _breed)
  {
    breed = _breed;
  }

  public void setWorkPhone(String workPhone)
  {
    this.workPhone = workPhone;
  }

  public String getWorkPhone()
  {
    return workPhone;
  }

  public void setHomePhone(String homePhone)
  {
    this.homePhone = homePhone;
  }

  public String getHomePhone()
  {
    return homePhone;
  }

  public void setCellPhone(String cellPhone)
  {
    this.cellPhone = cellPhone;
  }

  public String getCellPhone()
  {
    return cellPhone;
  }

  public void setVetPhone(String vetPhone)
  {
    this.vetPhone = vetPhone;
  }

  public String getVetPhone()
  {
    return vetPhone;
  }

  public void setEmail(String email)
  {
    this.email = email;
  }

  public String getEmail()
  {
    return email;
  }

  public void setTagCode(String tagCode)
  {
    this.tagCode = tagCode;
  }

  public String getTagCode()
  {
    return tagCode;
  }

  public void setName(String name)
  {
    this.name = name;
  }

  public String getName()
  {
    return name;
  }

  public void setPreferredFood(String preferredFood)
  {
    this.preferredFood = preferredFood;
  }

  public String getPreferredFood()
  {
    return preferredFood;
  }

  public void setMedicalNeeds(String medicalNeeds)
  {
    this.medicalNeeds = medicalNeeds;
  }

  public String getMedicalNeeds()
  {
    return medicalNeeds;
  }


  public void setVetName(String vetName)
  {
    this.vetName = vetName;
  }

  public String getVetName()
  {
    return vetName;
  }

  public void setVetAddress(String vetAddress)
  {
    this.vetAddress = vetAddress;
  }

  public String getVetAddress()
  {
    return vetAddress;
  }

  public void setReward(int reward)
  {
    this.reward = reward;
  }

  public int getReward()
  {
    return reward;
  }

  public void setPassword(String password)
  {
    this.password = password;
  }

  public String getPassword()
  {
    return password;
  }

  public void setMedications(String medications)
  {
    this.medications = medications;
  }

  public String getMedications()
  {
    return medications;
  }
}
