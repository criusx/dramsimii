package evDemo;

public class spectrumInfo
{
  private boolean exists;
  private int[] dataArray;
  private String RFIDNum;

  public spectrumInfo()
  {
    this.exists = false;
  }

  public void setExists(boolean exists)
  {
    this.exists = exists;
  }

  public boolean isExists()
  {
    return exists;
  }

  public void setDataArray(int[] dataArray)
  {
    this.dataArray = dataArray;
  }

  public int[] getDataArray()
  {
    return dataArray;
  }

  public void setRFIDNum(String rFIDNum)
  {
    this.RFIDNum = rFIDNum;
  }

  public String getRFIDNum()
  {
    return RFIDNum;
  }
}
