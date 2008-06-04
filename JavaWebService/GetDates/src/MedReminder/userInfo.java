package MedReminder;

/**
 * Used to create an initial account
 */
public class userInfo
{
  private boolean exists;
  private String email;
  private String password;
  private String name;
  private byte[] image;
  private String phoneNumber;
  private String provider;


  public userInfo()
  {
  }

  public userInfo(boolean exists)
  {
    this.exists = exists;
  }

  public void setExists(boolean exists)
  {
    this.exists = exists;
  }

  public boolean isExists()
  {
    return exists;
  }

  public void setEmail(String email)
  {
    this.email = email;
  }

  public String getEmail()
  {
    return email;
  }

  public void setPassword(String password)
  {
    this.password = password;
  }

  public String getPassword()
  {
    return password;
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
