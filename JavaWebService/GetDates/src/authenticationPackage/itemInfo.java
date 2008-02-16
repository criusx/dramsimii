package authenticationPackage;

/**
 * contains information about items being checked for authenticity
 */
public class itemInfo
{
  private String RFIDNum;
  private String description;
  private boolean authenticated;
  private boolean retryNeeded;

  public itemInfo()
  {
    authenticated = false;
    RFIDNum = "";
    description = "";
    retryNeeded = false;
  }

  public itemInfo(String _RFIDNum, String _description, 
                  boolean _authenticated, boolean RetryNeeded)
  {
    RFIDNum = _RFIDNum;
    description = _description;
    authenticated = _authenticated;
    retryNeeded = RetryNeeded;
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
}
