package winepackage;

public class wineBottle
{
  private int year;
  private boolean authenticated;
  private boolean exists;
  private String vineyard;
  private String type;
  private String origin;
  private String review;
  private String rfidNum;
  private byte[] image;
  private int lookedUpTimes;
  private boolean retryNeeded;

  public wineBottle()
  {
    exists = false;
    lookedUpTimes = 0;
  }
  
  public wineBottle(String _rfidNum)
  {
    rfidNum = _rfidNum;
    exists = false;
    lookedUpTimes = 0;
  }

  public wineBottle(int _year, boolean _authenticated, String _vineyard, 
                    String _type, String _origin, byte[] _image, 
                    String _review, String _rfidNum, int _lookedUpTimes, boolean RetryNeeded)
  {
    rfidNum = _rfidNum;
    authenticated = _authenticated;
    year = _year;
    vineyard = _vineyard;
    type = _type;
    origin = _origin;
    image = _image;
    review = _review;
    exists = true;
    lookedUpTimes = _lookedUpTimes;
    retryNeeded = RetryNeeded;
  }

  // called when the bottle does not exist, but it has been tried a number of times

  public wineBottle(String _rfidNum, int _lookedUpTimes)
  {
    rfidNum = _rfidNum;
    lookedUpTimes = _lookedUpTimes;
    exists = false;
  }

  public void setLookedUpTimes(int _lut)
  {
    lookedUpTimes = _lut;
  }

  public int getLookedUpTimes()
  {
    return lookedUpTimes;
  }

  public void setRfidNum(String _rfidNum)
  {
    rfidNum = _rfidNum;
  }

  public String getRfidNum()
  {
    return rfidNum;
  }

  public void setReview(String _review)
  {
    review = _review;
  }

  public String getReview()
  {
    return review;
  }

  public void setAuthenticated(boolean _authenticated)
  {
    authenticated = _authenticated;
  }

  public boolean isAuthenticated()
  {
    return authenticated;
  }

  public void setExists(boolean _exists)
  {
    exists = _exists;
  }

  public boolean isExists()
  {
    return exists;
  }

  public void setYear(int id)
  {
    year = id;
  }

  public int getYear()
  {
    return year;
  }

  public void setVineyard(String firstName)
  {
    this.vineyard = firstName;
  }

  public String getVineyard()
  {
    return vineyard;
  }

  public byte[] getImage()
  {
    return image;
  }

  public void setImage(byte[] _image)
  {
    image = _image;
  }

  public void setType(String lastName)
  {
    this.type = lastName;
  }

  public String getType()
  {
    return type;
  }

  public void setOrigin(String title)
  {
    this.origin = title;
  }

  public String getOrigin()
  {
    return origin;
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
