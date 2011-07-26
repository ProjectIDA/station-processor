/**
 * 
 */
package uforsplot;


/**
 * @author Frank Shelly
 *
 */
public class uforsplot
{
	private static void ShowHelp()
	{
		System.out.print(
				"USAGE: seedsec\n" +
				"       seedsec help\n" +
				"       seedsec archive <host> <port> <station>.[<location>-]<channel> <savefile>\n" +
				"\n" +
				"	<host>          Host name or IP address\n" +
				"	<port>          4003 or other port number\n" +
				"	<station>       Station name\n" +
				"	<location>      Location code, missing means blank location\n" +
//				"	                Can use ? and * for wildcarding\n" +
				"	<channel>       3 Character channel name\n" +
//				"	                Can use ? and * for wildcarding\n" +
				" <savefile>      Append all data to this file\n" +
				"\n" +
				"seedsec with no arguments brings up a plot user interface to plot liss feed" +
				"\n" +
				"seedsec archive saves a liss feed to an archive file only\n" +
				"\n"
			);

	} // ShowHelp()

	/**
	 * @param args
	 */
	public static void main(String[] args)
	{
		// No arguments means run the display window
		if (args.length == 0)
		{
			prefs = new preferences();

			new display();
		}

		else // argument count is wrong, show help
		{
			ShowHelp();
		}
	} // main()

	public static preferences prefs;	
} // class seedsec
