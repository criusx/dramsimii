package MedReminder;

/**
 * Used to create an initial account
 */
public class userInfo
{
  private String email;
  private String name;
  private byte[] image;
  private String phoneNumber;
  private String provider;

  public userInfo()
  {
  }  
  
  public userInfo(String email, String name, byte[] image, String phoneNumber, String provider)
  {
    this.email = email;
    this.name = name;
    this.image = image;
    this.phoneNumber = phoneNumber;
    this.provider = provider;
  }

  public void setEmail(String email)
  {
    this.email = email;
  }

  public String getEmail()
  {
    return email;
  }


  public void setName(String name)
  {
    this.name = name;
  }

  public String getName()
  {
    return name;
  }

  public void setImage(byte[] image)
  {
    this.image = image;
  }

  public byte[] getImage()
  {
    return image;
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
}
