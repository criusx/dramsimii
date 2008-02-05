package assayPackage;

public class assayReport
{
  private String sessionID;
  private int selectedIndex;

  public assayReport()
  {
  }

  public void setSessionID(String sessionID)
  {
    this.sessionID = sessionID;
  }

  public String getSessionID()
  {
    return sessionID;
  }

  public void setSelectedIndex(int selectedIndex)
  {
    this.selectedIndex = selectedIndex;
  }

  public int getSelectedIndex()
  {
    return selectedIndex;
  }
}
