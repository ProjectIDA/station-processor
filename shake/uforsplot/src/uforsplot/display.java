package uforsplot;

import java.awt.BorderLayout;
import java.awt.Container;
import java.awt.Dimension;
import java.awt.Toolkit;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.FocusEvent;
import java.awt.event.FocusListener;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import java.awt.event.WindowListener;
import java.io.File;
import java.util.Date;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.TimeUnit;

import javax.swing.BoxLayout;
import javax.swing.JFileChooser;
import javax.swing.JFrame;
import javax.swing.JMenu;
import javax.swing.JMenuBar;
import javax.swing.JMenuItem;
import javax.swing.JPanel;
import javax.swing.JTextField;
import javax.swing.SwingWorker;
import javax.swing.border.EmptyBorder;

import seed.IllegalSeednameException;
import seed.MiniSeed;
import seed.SteimException;

public class display
{
	private PlotFrame frame;

	public display()
	{
		WindowListener listener = new Terminator();
		frame = new PlotFrame();
		frame.setDefaultCloseOperation(JFrame.HIDE_ON_CLOSE);
		frame.addWindowListener(listener);
		frame.setVisible(true);
	} // display() constructor

	/**
	 * Called when the GUI exit button is clicked. Allows GUI to save persistent
	 * state information for next launch.
	 */
	class Terminator extends WindowAdapter
	{
		public void windowClosing(WindowEvent e)
		{
			frame.SavePrefs(); // SavePrefs also exits program
		}
	} // class Terminator
} // class display

/**
 * Top level frame for display GUI, everything else fits inside.
 */
class PlotFrame extends JFrame implements ActionListener, FocusListener
{
	private static final long serialVersionUID = 1L;

	public PlotFrame()
	{
		prefs = new preferences();
		setTitle("uFors Rotation rate data plotter");
		setPreferredSize(new Dimension(prefs.GetWidth(), prefs.GetHeight()));
		setBounds(prefs.GetOriginX(), prefs.GetOriginY(), 
				prefs.GetWidth(), prefs.GetHeight());

		// Set up saved preference fields
		localSeedFileFrame = new JFileChooser();
		localSeedFileFrame.setCurrentDirectory(new File(prefs.GetLocalDir()));
		hostField = prefs.GetHostname();
		channel1Field = prefs.GetChannel1();
		channel2Field = prefs.GetChannel2();
		channel3Field = prefs.GetChannel3();
		locationField = prefs.GetLocation();
		portField = Integer.toString(prefs.GetPort());
		secondsDuration = prefs.GetSecondsDuration();
		minRange = prefs.GetMinRange();

		// ======== this ========
		Container contentPane = getContentPane();
		contentPane.setLayout(new BoxLayout(contentPane, BoxLayout.Y_AXIS));

		// ======== mainMenuBar ========
		{
			mainMenuBar = new JMenuBar();
			fileJMenu = new JMenu();
			exitJMenu = new JMenuItem();
			setupJMenu = new JMenuItem();

			mainMenuBar.setPreferredSize(new Dimension(166, 21));

			// ======== fileJMenu ========
			{
				fileJMenu.setText(" File  ");

				// ---- setupJMenu ----
				setupJMenu.setText("Setup");
				setupJMenu.addActionListener(this);
				fileJMenu.add(setupJMenu);
				
				// ---- exitJMenu ----
				exitJMenu.setText("Exit");
				exitJMenu.addActionListener(this);
				fileJMenu.add(exitJMenu);
			} // fileJMenu
			mainMenuBar.add(fileJMenu);

			// All done setting up menu bar
			setJMenuBar(mainMenuBar);
		} // mainMenuBar

		// ======== graphViewJPanel ========
    String[] chanArray = { channel1Field, channel2Field, channel3Field };
		timeChartPlot = new TimeChartPlotter(secondsDuration, minRange, 
		    chanArray, station, network, locationField);
		graphViewJPanel = timeChartPlot.createTimePanel();
		graphViewJPanel.setMinimumSize(new Dimension(600, 300));
		graphViewJPanel.setPreferredSize(new Dimension(prefs.GetWidth(),
		    prefs.GetHeight()));
		graphViewJPanel.setBorder(new EmptyBorder(10, 10, 10, 10));
		add(graphViewJPanel, BorderLayout.CENTER);

		// Status field
		statusField = new JTextField();
		statusField.setEditable(false);
		statusField.setText("Initializing...");
		add(statusField, BorderLayout.SOUTH);

		// Start the data Transfer going
		transferTask = new TransferTask();
		transferTask.execute();
	}// constructor PlotFrame()

	class TransferTask extends SwingWorker<Object, Object>
	{
		/*
		 * Main task. Executed in background thread.
		 */
		public Object doInBackground()
		{
			Transfer();
			return null;
		} // doInBackground()

		/*
		 * Executed in event dispatching thread
		 */
		public void done()
		{
			setCursor(null); // turn off the wait cursor
			Toolkit.getDefaultToolkit().beep();
		} // done()
	} // class TransferTask

	public void Transfer()
	{
		//
		// First get a filtered directory listing for error checking
		//
		String hostname = hostField;
		int port = Integer.parseInt(portField);
    int iCount = 0;
		int iSeedCount;
		int iMatchCount=0;
		int clock_quality=-1;
		BlockingQueue<SeedRawRecord> queue;
		SeedRawRecord head;

		// loop until transfer is canceled
		while (!bCancel)
		{
			if (bResetConnection)
			{
				// Build up an id string
				hostname = hostField;
				port = Integer.parseInt(portField);
				iMatchCount = 0;
				iSeedCount = 0;
		    // Build up an id string
				bResetConnection = false;
			} // Somebody requested that we reset network connection
			
      id1 = String.format("%-3.3s%-2.2s",
          channel1Field, locationField);
      
      id2 = String.format("%-3.3s%-2.2s",
          channel2Field, locationField);
      
      id3 = String.format("%-3.3s%-2.2s",
          channel3Field, locationField);
      
			statusField.setText("Connecting to host " + hostname + ":" + port + "\n");

			// Get data for this channel and time period
			LISSsocket getSeedThread = new LISSsocket(hostname, port);
			getSeedThread.start();

			// Give time for a connection to be attempted
			try
			{
				Thread.sleep(2000);
			} catch (InterruptedException e)
			{
				statusField.setText(
						"Error Transfer(): Sleep failed waiting for connection");
				break;
			}			
			if (getSeedThread.IsConnected())
			{
				statusField.setText("Connected to host " + hostname + ":" + port + "\n");
			}
			else
			{
				if (getSeedThread.IsBadHost())
				{
					statusField.setText("Unknown host " + hostname + ":" + port + "\n");
					while (!bResetConnection)
					{
						try
						{
							Thread.sleep(5000);
						} catch (InterruptedException e)
						{
							statusField.setText(
									"Error Transfer(): Sleep failed waiting for connection");
							break;
						}			
					}
				} // hostname was bad
				else
					statusField.setText("Failed connection " + hostname + ":" + port + "\n");
				try
				{
					Thread.sleep(5000);
				} catch (InterruptedException e)
				{
					statusField.setText(
							"Error Transfer(): Sleep failed waiting for connection");
					break;
				}			
				continue;
			}
			
			queue = getSeedThread.GetQueue();

			while (!getSeedThread.Done() && getSeedThread.isAlive())
			{
				if (bCancel || bResetConnection)
				{
					if (!getSeedThread.GetCancel())
					{
						statusField.setText("Closing network connection");
						getSeedThread.SetCancel(true);
					}
					continue;
				}
				
				try
        {
	        head = queue.poll(1000, TimeUnit.MILLISECONDS);
        } catch (InterruptedException e1)
        {
        	// Unexpected error removing item from queue
					statusField.setText("Error removing record from read queue");
					e1.printStackTrace();
        	continue;
        }
        // Check for timeout
        if (head == null)
        {
        	continue;
        }
        
				if (bCancel || bResetConnection)
				{
					if (!getSeedThread.GetCancel())
					{
						statusField.setText("Closing network connection");
						getSeedThread.SetCancel(true);
					}
					continue;
				}
				
				iCount++;
				iSeedCount = getSeedThread.GetRecordCount();

				// Pull the seed records off of the queue
				try
        {
	        seedRecord = new MiniSeed(head.GetRecord());
        } catch (IllegalSeednameException e1)
        {
	        // Problems with seed record, warn and skip
    			statusField.setText("Error in seed record, skipping");
    			continue;
        }

				// We have a new seed record see if it is the right channel
				newCCCLL = seedRecord.getSeedName().substring(7);
				if (( id1.compareToIgnoreCase(newCCCLL) == 0
				   || id2.compareToIgnoreCase(newCCCLL) == 0
           || id3.compareToIgnoreCase(newCCCLL) == 0)
						&& (seedRecord.getNsamp() > 0))
				{
					iMatchCount++;

      		int newData[];
      		int iChan;
      		// This is what we wanted, plot it
      		try
      		{
      			newData = seedRecord.decomp();
      		}
      		catch(SteimException e)
      		{
      			newData = null;
      			statusField.setText("Steim Exception raised, aborting");
      	    System.err.println("Steim Exception raised.");
      	    e.printStackTrace();
      	    System.exit(1);
      		}
					
          if (id1.compareToIgnoreCase(newCCCLL) == 0)
            iChan = 0;
          else if (id2.compareToIgnoreCase(newCCCLL) == 0)
            iChan = 1;
          else 
            iChan = 2;
          if (station.length() == 0 || network.length() == 0)
					{
						// We need to label the title according to the station
      			network = seedRecord.getSeedName().substring(0,2);
						station = seedRecord.getSeedName().substring(2,7).trim();
						timeChartPlot.SetTitle(station, network, locationField);
					}

          byte [] b1001 = seedRecord.getBlockette1001();
          if (b1001 != null)
            clock_quality = b1001[4];
          else
            clock_quality = -1;
          
					if (newData != null)
					  timeChartPlot.AddNewData(newData, iChan,
					      seedRecord.getRate(), 
					      new Date(seedRecord.getGregorianCalendar().getTimeInMillis()),
					      clock_quality);

				} // Seed record matched the channel we are plotting
				if (iSeedCount > 0)
					statusField.setText("Records " + iMatchCount + "/"+ iSeedCount);

			} // loop until data collection thread says it is done

			statusField.setText("Disconnected after " + getSeedThread.GetRecordCount()
			    + " records");
			try
			{
				Thread.sleep(5000);
			} catch (InterruptedException e)
			{
				statusField
				    .setText("Error Transfer(): Sleep failed waiting for SeedThread\n");
				return;
			}
		} // while not canceled
	} // Transfer()

	public void actionPerformed(ActionEvent e)
	{
		String command = e.getActionCommand();
		Object source = e.getSource();
		if (source == exitJMenu)
		{
			SavePrefs(); // SavePrefs also exits program
		} else if (source == setupJMenu)
		{
			setup dialog = new setup(this, prefs.GetHostname(), prefs.GetPort(), 
					prefs.GetChannel1(), prefs.GetChannel2(), prefs.GetChannel3(), 
					prefs.GetLocation(), prefs.GetSecondsDuration(), prefs.GetMinRange());
			dialog.setVisible(true);
			if (dialog.GetNetChange())
			{
				hostField = dialog.GetHostname();
				portField = Integer.toString(dialog.GetPort());
				bResetConnection = true;
			}
			
			if (dialog.GetDisplayChange() || dialog.GetSmallChange() 
					|| dialog.GetNetChange())
			{
				// Something modified so save changes
				channel1Field = dialog.GetChannel1();
        channel2Field = dialog.GetChannel2();
        channel3Field = dialog.GetChannel3();
				locationField = dialog.GetLocation();
				secondsDuration = dialog.GetSecondsDuration();
				minRange = dialog.GetMinRange();
				
				// Remember any changes to preferences
				prefs.SetLocalDir(localSeedFileFrame.getCurrentDirectory()
				    .getAbsolutePath());
				prefs.SetHostname(hostField);
				prefs.SetPort(Integer.valueOf(portField).intValue());
				prefs.SetChannel1(channel1Field);
        prefs.SetChannel2(channel2Field);
        prefs.SetChannel3(channel3Field);
				prefs.SetLocation(locationField);
				prefs.SetSecondsDuration(secondsDuration);
				prefs.SetMinRange(minRange);
				prefs.SavePreferences();
				if (dialog.GetDisplayChange() || dialog.GetNetChange())
				{
					station = "";
					network = "";
		      id1 = String.format("%-3.3s%-2.2s",
		          channel1Field, locationField);
		      id2 = String.format("%-3.3s%-2.2s",
		          channel2Field, locationField);
		      id3 = String.format("%-3.3s%-2.2s",
		          channel3Field, locationField);
		 					Dimension rememberSize = getSize();
					remove(graphViewJPanel);
			    String[] chanArray = { channel1Field, channel2Field, channel3Field };
			    timeChartPlot = new TimeChartPlotter(secondsDuration, minRange, 
			        chanArray, station, network, locationField);
					graphViewJPanel = timeChartPlot.createTimePanel();
					graphViewJPanel.setMinimumSize(new Dimension(600, 300));
					graphViewJPanel.setPreferredSize(rememberSize);
					graphViewJPanel.setBorder(new EmptyBorder(10, 10, 10, 10));
					remove(statusField);
					add(graphViewJPanel, BorderLayout.CENTER);
					add(statusField, BorderLayout.SOUTH);
					setPreferredSize(rememberSize);
					pack();
					repaint();		
				}
				
				if (dialog.GetSmallChange())
				{
					timeChartPlot.SetTimeSpanSeconds(secondsDuration);
					timeChartPlot.SetMinRange(minRange);
				}
			}
		} else if (command.compareTo("Exit") == 0)
		{
			SavePrefs(); // SavePrefs also exits program
		} else
		{
			System.err.println("Unmanaged actionPerformed " + command);
		}
	} // actionPerformed()

	public void focusGained(FocusEvent e)
	{
//		JFormattedTextField field = (JFormattedTextField) e.getComponent();

		// Save the current field value in case the user botches up the edit.
		// This allows us to restore the prior value upon field exit
//		saveFocusString = field.getText();
	} // focusGained

	public void focusLost(FocusEvent e)
	{

	} // focusLost()

	/**
	 * Saves persistent state information so that it can be retrieved the next
	 * time the program is run.
	 */
	public void SavePrefs()
	{
  	System.err.println("DEBUG SavePrefs entered, window: " 
  			+ this.getX() + "," + this.getY() + "; " 
  			+ this.getWidth() + "," + this.getHeight());
		prefs.SetLocalDir(localSeedFileFrame.getCurrentDirectory()
		    .getAbsolutePath());
		prefs.SetHostname(hostField);
		prefs.SetPort(Integer.valueOf(portField).intValue());
		prefs.SetChannel1(channel1Field);
    prefs.SetChannel2(channel2Field);
    prefs.SetChannel3(channel3Field);
		prefs.SetLocation(locationField);
		prefs.SetSecondsDuration(secondsDuration);
		prefs.SetMinRange(minRange);
		prefs.SetOriginX(this.getX());
		prefs.SetOriginY(this.getY());
		prefs.SetHeight(this.getHeight());
		prefs.SetWidth(this.getWidth());

		// Remember any changes to preferences
		prefs.SavePreferences();
		bCancel = true;
		System.exit(0);
	} // SavePrefs()

	final static int            GAP            = 10;

	private String              channel1Field;
  private String              channel2Field;
  private String              channel3Field;
	private String              locationField;
	private String              hostField;
	private String              portField;
	private JFileChooser        localSeedFileFrame;
	private preferences         prefs;
	
	private String							station="";
	private String							network="";
	private int                 secondsDuration;
	private int                 minRange;

	// Menu
	private JMenuBar            mainMenuBar;
	private JMenu               fileJMenu;
	private JMenuItem           exitJMenu;
	private JMenuItem           setupJMenu;

	private JPanel              graphViewJPanel;
	private TimeChartPlotter    timeChartPlot;
	private JTextField          statusField;

	private boolean             bCancel        = false;
	private boolean							bResetConnection = false;
	private TransferTask        transferTask;

  private String              id1;
  private String              id2;
  private String              id3;
  private String              newCCCLL;


	private MiniSeed            seedRecord;

} // class PlotFrame
