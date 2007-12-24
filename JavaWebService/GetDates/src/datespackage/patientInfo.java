package datespackage;

public class patientInfo
{
  private String rfidNum;
  private String name;
  private String description;
  private byte[] image;
  
  public patientInfo(String _rfidNum, String _name, String _description, byte[] _image)
  {
    rfidNum = _rfidNum;
    name = _name;
    description = _description;
    image = _image;
  }
  
  public patientInfo()
  {
    name = "Not available";
    description = "None.";
  }
  
  public void setRfidNum(String _rfidNum)
  {
    rfidNum = _rfidNum;
  }

  public String getRfidNum()
  {
    return rfidNum;
  }
  
  public void setName(String _name)
  {
    name = _name;
  }

  public String getName()
  {
    return name;
  }
  
  public void setDescription(String _description)
  {
    description = _description;
  }

  public String getDescription()
  {
    return description;
  }
  
  public byte[] getImage()
  {
    return image;
  }

  public void setImage(byte[] _image)
  {
    image = _image;
  }
  
}
