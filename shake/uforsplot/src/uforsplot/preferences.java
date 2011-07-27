package uforsplot;

import java.io.File;
import java.util.prefs.Preferences;


public class preferences
{
	private static boolean initialized=false;
	private static boolean saveData;
	private static String station;
	private static String network;
	private static String hostname;
	private static String channel1;
  private static String channel2;
  private static String channel3;
	private static String location;
	private static int    port;
	private static int    secondsDuration;
	private static int    minRange;
	private static String localDir;
	private static int    height;
	private static int    width;
	private static int    originX;
	private static int		originY;
	private Preferences node;

	public preferences()
	{
		Preferences root = Preferences.userRoot();
		node = root.node("/uforsplot");
		
		if (!initialized)
		{
			saveData = node.getBoolean("saveData", false);
			station = node.get("station", "ASPX");
			network = node.get("network", "XX");
			hostname = node.get("hostname", "136.177.121.196");
			channel1 = node.get("channel1", "EJ1");
      channel2 = node.get("channel2", "EJ2");
      channel3 = node.get("channel3", "EJZ");
			location = node.get("location", "00");
			port = node.getInt("port", 4000);
			secondsDuration = node.getInt("duration", 240);
			minRange = node.getInt("minrange", 500000);
			localDir = node.get("localDir", "." + File.pathSeparatorChar);
			height = node.getInt("height", 500);
			width  = node.getInt("width", 800);
			originX = node.getInt("originX", 50);
			originY = node.getInt("originY", 50);
			initialized = true;
		}
	} // preferences()
	
	// Call to save any user modified preferences
	public void SavePreferences()
	{
		node.putBoolean("saveData", saveData);
		node.put("station", station);
		node.put("network", network);
		node.put("hostname", hostname);
		node.put("channel1", channel1);
    node.put("channel2", channel2);
    node.put("channel3", channel3);
		node.put("location", location);
		node.putInt("port", port);
		node.putInt("duration", secondsDuration);
		node.putInt("minrange", minRange);
		node.put("localDir", localDir);
		node.putInt("height", height);
		node.putInt("width", width);
		node.putInt("originX", originX);
		node.putInt("originY", originY);
	} // dispose()
	
	public String GetStation()
	{
		return station;
	}
	
	public String GetNetwork()
	{
		return network;
	}
	
	public String GetHostname()
	{
		return hostname;
	}
	
	public String GetChannel1()
	{
		return channel1;
	}
	
  public String GetChannel2()
  {
    return channel2;
  }
  
  public String GetChannel3()
  {
    return channel3;
  }
  
	public String GetLocation()
	{
		return location;
	}
	
	public String GetLocalDir()
	{
		return localDir;
	}
	
	public int GetPort()
	{
		return port;
	}
	
	public int GetSecondsDuration()
	{
		return secondsDuration;
	}
	
  public int GetMinRange()
  {
    return minRange;
  }
  
	public boolean GetSaveData()
	{
		return saveData;
	}
	
	public int GetHeight()
	{
		return height;
	}
	
	public int GetWidth()
	{
		return width;
	}
	
	public int GetOriginX()
	{
		return originX;
	}
	
	public int GetOriginY()
	{
		return originY;
	}
	
	public void SetStation(String newStation)
	{
		station = newStation;
	}
	
	public void SetNetwork(String newNetwork)
	{
		network = newNetwork;
	}
	
	public void SetHostname(String newHostname)
	{
		hostname = newHostname;
	}
	
	public void SetChannel1(String newChannel)
	{
		channel1 = newChannel;
	}
	
  public void SetChannel2(String newChannel)
  {
    channel2 = newChannel;
  }
  
  public void SetChannel3(String newChannel)
  {
    channel3 = newChannel;
  }
  
	public void SetLocation(String newLocation)
	{
		location = newLocation;
	}
	
	public void SetLocalDir(String newLocalDir)
	{
		localDir = newLocalDir;
	}
	
	public void SetPort(int newPort)
	{
		port = newPort;
	}
	
	public void SetSecondsDuration(int newSecondsDuration)
	{
		secondsDuration = newSecondsDuration;
	}
	
  public void SetMinRange(int newMinRange)
  {
    minRange = newMinRange;
  }
  
	public void SetHeight(int newHeight)
	{
		height = newHeight;
	}
	
	public void SetWidth(int newWidth)
	{
		width = newWidth;
	}
	
	public void SetOriginX(int x)
	{
		originX = x;
	}
	
	public void SetOriginY(int y)
	{
		originY = y;
	}
	
	public void SetSaveData(boolean newSaveData)
	{
		saveData = newSaveData;
	}
	
} // class preferences
