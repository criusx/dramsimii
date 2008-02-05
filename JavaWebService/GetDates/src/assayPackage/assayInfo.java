package assayPackage;

import java.lang.reflect.Array;

public class assayInfo
{
  private String rfidNum;
  private String sessionID;
  private int timer;
  private byte[] descriptionImage;
  private assayImage[] resultImages;
  private String beforeMessage;
  private String afterMessage;
  private boolean exists;

  public assayInfo()
  {
    exists = false;
  }

  public assayInfo(boolean value)
  {
    exists = true;
  }


  public void setSessionID(String sessionID)
  {
    this.sessionID = sessionID;
  }

  public String getSessionID()
  {
    return sessionID;
  }

  public void setTimer(int timer)
  {
    this.timer = timer;
  }

  public int getTimer()
  {
    return timer;
  }


  public void setBeforeMessage(String beforeMessage)
  {
    this.beforeMessage = beforeMessage;
  }

  public String getBeforeMessage()
  {
    return beforeMessage;
  }

  public void setAfterMessage(String afterMessage)
  {
    this.afterMessage = afterMessage;
  }

  public String getAfterMessage()
  {
    return afterMessage;
  }

  public void setDescriptionImage(byte[] descriptionImage)
  {
    this.descriptionImage = descriptionImage;
  }

  public byte[] getDescriptionImage()
  {
    return descriptionImage;
  }

  public void setExists(boolean exists)
  {
    this.exists = exists;
  }

  public boolean isExists()
  {
    return exists;
  }

  public void setResultImages(assayImage[] resultImages)
  {
    this.resultImages = resultImages;
  }

  public assayImage[] getResultImages()
  {
    return resultImages;
  }

  public void setRfidNum(String rfidNum)
  {
    this.rfidNum = rfidNum;
  }

  public String getRfidNum()
  {
    return rfidNum;
  }
}