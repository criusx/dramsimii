package ShoePackage;

public class shoePair
{
  private String RFIDNumA;
  private String RFIDNumB;
  private String description;
  private boolean authenticated;
  private boolean retryNeeded;
  private byte[] image;
  private boolean exists;
  private String ownerGUID;
  
  public shoePair()
  {
    exists = false;
    retryNeeded = true;
  }

  public void setRFIDNumA(String rFIDNumA)
  {
    this.RFIDNumA = rFIDNumA;
  }

  public String getRFIDNumA()
  {
    return RFIDNumA;
  }

  public void setRFIDNumB(String rFIDNumB)
  {
    this.RFIDNumB = rFIDNumB;
  }

  public String getRFIDNumB()
  {
    return RFIDNumB;
  }

  public void setDescription(String description)
  {
    this.description = description;
  }

  public String getDescription()
  {
    return description;
  }

  public void setAuthenticated(boolean authenticated)
  {
    this.authenticated = authenticated;
  }

  public boolean isAuthenticated()
  {
    return authenticated;
  }

  public void setRetryNeeded(boolean retryNeeded)
  {
    this.retryNeeded = retryNeeded;
  }

  public boolean isRetryNeeded()
  {
    return retryNeeded;
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

  public void setOwnerGUID(String ownerGUID)
  {
    this.ownerGUID = ownerGUID;
  }

  public String getOwnerGUID()
  {
    return ownerGUID;
  }
}
