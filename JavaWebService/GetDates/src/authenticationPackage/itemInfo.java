package authenticationPackage;

public class itemInfo
{
  private String RFIDNum;
  private String description;
  private boolean authenticated;

  public itemInfo()
  {
    authenticated = false;
    RFIDNum = "";
    description = "";
  }

  public itemInfo(String _RFIDNum, String _description, 
                  boolean _authenticated)
  {
    RFIDNum = _RFIDNum;
    description = _description;
    authenticated = _authenticated;
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
}
