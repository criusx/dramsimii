package dhlDemo;

public class packageInfo
{
  private boolean exists;
  private String RFIDNum;
  private String trackingNumber;
  private String deliveryAddress;
  private String deliveryAddress2;
  private String city;
  private String state;
  private String zip;
  private String recipient;
  private String recipientEmail;
  private String recipientPager;
  private float latitude;
  private float longitude;
  private String country;

  public packageInfo()
  {
    exists = false;
  }


  public void setExists(boolean exists)
  {
    this.exists = exists;
  }

  public boolean isExists()
  {
    return exists;
  }

  public void setRFIDNum(String rFIDNum)
  {
    this.RFIDNum = rFIDNum;
  }

  public String getRFIDNum()
  {
    return RFIDNum;
  }

  public void setTrackingNumber(String trackingNumber)
  {
    this.trackingNumber = trackingNumber;
  }

  public String getTrackingNumber()
  {
    return trackingNumber;
  }

  public void setDeliveryAddress(String deliveryAddress)
  {
    this.deliveryAddress = deliveryAddress;
  }

  public String getDeliveryAddress()
  {
    return deliveryAddress;
  }

  public void setRecipientEmail(String recipientEmail)
  {
    this.recipientEmail = recipientEmail;
  }

  public String getRecipientEmail()
  {
    return recipientEmail;
  }

  public void setRecipientPager(String recipientPager)
  {
    this.recipientPager = recipientPager;
  }

  public String getRecipientPager()
  {
    return recipientPager;
  }

  public void setLatitude(float latitude)
  {
    this.latitude = latitude;
  }

  public float getLatitude()
  {
    return latitude;
  }

  public void setLongitude(float longitude)
  {
    this.longitude = longitude;
  }

  public float getLongitude()
  {
    return longitude;
  }

  public void setDeliveryAddress2(String deliveryAddress2)
  {
    this.deliveryAddress2 = deliveryAddress2;
  }

  public String getDeliveryAddress2()
  {
    return deliveryAddress2;
  }

  public void setRecipient(String recipient)
  {
    this.recipient = recipient;
  }

  public String getRecipient()
  {
    return recipient;
  }

  public void setCountry(String country)
  {
    this.country = country;
  }

  public String getCountry()
  {
    return country;
  }

  public void setCity(String city)
  {
    this.city = city;
  }

  public String getCity()
  {
    return city;
  }

  public void setState(String state)
  {
    this.state = state;
  }

  public String getState()
  {
    return state;
  }

  public void setZip(String zip)
  {
    this.zip = zip;
  }

  public String getZip()
  {
    return zip;
  }
}
