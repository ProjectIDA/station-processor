package uforsplot;

import java.awt.Color;
import java.text.SimpleDateFormat;
import java.util.Calendar;
import java.util.Date;
import java.util.TimeZone;

import javax.swing.JPanel;

import org.jfree.chart.ChartFactory;
import org.jfree.chart.JFreeChart;
import org.jfree.chart.axis.DateAxis;
import org.jfree.chart.axis.NumberAxis;
import org.jfree.chart.plot.XYPlot;
import org.jfree.chart.renderer.xy.XYLineAndShapeRenderer;
import org.jfree.data.time.Millisecond;
import org.jfree.data.time.TimeSeries;
import org.jfree.data.time.TimeSeriesCollection;
import org.jfree.data.xy.XYDataset;

/**
 * Plots seismic time series data
 */

/**
 * @author fshelly
 * 
 */
public class TimeChartPlotter extends JPanel
{
	/**
   * 
   */
  private static final long serialVersionUID = 1L;

	public TimeChartPlotter(int timeSpanSeconds, int newMinRange, 
	    String [] channel, String newStation, String newNetwork,String newLocation)
	{
		iTimeSpanSeconds = timeSpanSeconds;
		minRange = newMinRange;
		station = newStation;
		network = newNetwork;
		this.channel = channel;
		location = newLocation;
			  
		TimeZone.setDefault(timeZone);
	}

	private XYDataset createDataset()
	{
		dataset = new TimeSeriesCollection(timeZone);
		for (int i = 1; i <= iRowCount; i++)
		{
			TimeSeries series = new TimeSeries(channel[i-1], Millisecond.class);
			dataset.addSeries(series);
	    series.setMaximumItemCount(1000);
		}
		return dataset;
	}

	private JFreeChart createChartTime(XYDataset dataset)
	{
		chart = ChartFactory.createTimeSeriesChart(
				"uFors plot, waiting for data", // chart title
		    "UTC Time", // x axis label
		    "Response Velocity counts", // y axis label				
		    dataset, // data
		    true,    // legend
		    true,    // tooltips
		    false    // url
		    );

		chart.setBackgroundPaint(Color.white);
		XYPlot plot = chart.getXYPlot();

    final XYLineAndShapeRenderer renderer = new XYLineAndShapeRenderer();
    for(int i=0; i < iRowCount; i++){
      renderer.setSeriesLinesVisible(i, true);
      renderer.setSeriesShapesVisible(i, false);
    }
    plot.setRenderer(renderer);

    rangeAxis = (NumberAxis) plot.getRangeAxis();
    rangeAxis.setStandardTickUnits(NumberAxis.createIntegerTickUnits());
    rangeAxis.setAutoRange(true);
    rangeAxis.setAutoRangeMinimumSize(minRange);

		date_axis = (DateAxis) plot.getDomainAxis();
		date_axis.setDateFormatOverride(new SimpleDateFormat("HH:mm:ss"));
		Calendar cal = Calendar.getInstance(timeZone);
		Date endDate = cal.getTime();
		cal.add(Calendar.HOUR, -iTimeSpanSeconds/3600);
		cal.add(Calendar.SECOND, -(iTimeSpanSeconds % 3600));
		cal.set(Calendar.MILLISECOND, 0);
		Date startDate = cal.getTime();
		date_axis.setRange(startDate, endDate);
		
		return chart;
	} // createChartTime()

	public JPanel createTimePanel()
	{
		JFreeChart chart = createChartTime(createDataset());
		return new EHChartPanel(chart);
	}

	public JPanel recreateTimePanel()
	{
		JFreeChart chart = createChartTime(dataset);
		return new EHChartPanel(chart);
	}
	
	public void SetTitle(String newStation,
	    String newNetwork, String newLocation)
	{
		station = newStation;
		network = newNetwork;
		location = newLocation;
    String title = String.format("%s %s %s/",
        station, network, location);
    chart.setTitle(title);
	}
	
	public void SetTimeSpanSeconds(int newTimeSpanSeconds)
	{
		iTimeSpanSeconds = newTimeSpanSeconds;
	}
	
  public void SetMinRange(int newMinRange)
  {
    minRange = newMinRange;
    rangeAxis.setAutoRange(true);
    rangeAxis.setAutoRangeMinimumSize(minRange);
  }
  
	public void AddNewData(int newData[], int iChan, double dRate,
	    Date startTime, int clock_quality)
	{

		int i;
		TimeSeries series = dataset.getSeries(iChan);
		
    // Figure how much we have to compress data to keep plot performance up
    iCountsPer[iChan] = (int)(4*dRate*iTimeSpanSeconds) / 1000;
    if (iCountsPer[iChan] < 1)
      iCountsPer[iChan] = 1;  
    
    // We want to display the worst clock quality each second
    if (clock_quality < minClockQuality)
      minClockQuality = clock_quality;
    
		// See if this is the first time we've added data
		if (timeLastSum[iChan] == null)
		{
			iCount[iChan]=0;
      iMin[iChan] = 0;
      minVal[iChan] = newData[0];
      maxVal[iChan] = newData[0];
			timeLastSum[iChan] = startTime;
			midTime[iChan] = startTime;
		}
		else
		{
  		// Make any gaps in continuous data show up as blanks
			if (startTime.getTime() - timeLastSum[iChan].getTime() 
			      > 1000 + iCountsPer[iChan]*1000/dRate)
      {
				double gap = (startTime.getTime() - timeLastSum[iChan].getTime()) / 500;
				String gapstr = String.format("DEBUG gap %.1f sec in data starting at %s",
						              gap, timeLastSum[iChan].toString());
				System.err.println(gapstr);


				// Putting a null value at the start of the gap period works
				series.addOrUpdate(new Millisecond(timeLastSum[iChan]), null); 
      } // Handle gap in normally continuous data stream

		} // adding data to existing data
				
    for (i = 0; i < newData.length; i++)
    {
      // Where we are in iCountsPer
      iCount[iChan]++;

      // Get min and max values, order, and midpoint timetag
      if (iCount[iChan] == 1)
      {
        iMin[iChan] = 0;
        minVal[iChan] = newData[i];
        maxVal[iChan] = newData[i];
      } else
      {
        if (newData[i] < minVal[iChan])
        {
          iMin[iChan] = 1;
          minVal[iChan] = newData[i];
        } else if (newData[i] > maxVal[iChan])
        {
          iMin[iChan] = 0;
          maxVal[iChan] = newData[i];
        }
      }
      if (iCount[iChan] == iCountsPer[iChan] / 2)
        midTime[iChan] = new Date(startTime.getTime());

      if (iCount[iChan] >= iCountsPer[iChan])
      {
        if (iCountsPer[iChan] > 1)
        {
          if (iMin[iChan] == 0)
          {
            series.addOrUpdate(new Millisecond(midTime[iChan]), minVal[iChan]);
            series.addOrUpdate(new Millisecond(startTime), maxVal[iChan]);
          } else
          {
            series.addOrUpdate(new Millisecond(midTime[iChan]), maxVal[iChan]);
            series.addOrUpdate(new Millisecond(startTime), minVal[iChan]);
          }
        } else
        {
          series.addOrUpdate(new Millisecond(startTime), (double) newData[i]);
        }

        iCount[iChan] = 0;
        timeLastSum[iChan] = new Date(startTime.getTime());
      } // if we've merged the correct number of data points

      startTime = new Date(startTime.getTime() + (long) (1000.0 / dRate));
    } // loop through each new data item
	  
    if (tPlotEndSec.getTime()/1000 < startTime.getTime()/1000)
    {
      tPlotEndSec = new Date(startTime.getTime());
      tPlotStartSec = new Date(tPlotEndSec.getTime() - (iTimeSpanSeconds*1000));
      date_axis.setRange(tPlotStartSec, tPlotEndSec);
      
      String clock;
      if (minClockQuality < 0 || minClockQuality > 100)
      {
        clock = "Unknown";
      }
      else
      {
        clock = String.format("%d%%", minClockQuality);
      }
      minClockQuality = 100;
  
      String title = String.format("%s %s %s/%2.2s %s Clock Quality %s",
      		station, network, location, channel[iChan], 
      		tPlotEndSec.toString(), clock);
      chart.setTitle(title);
    }
	} // AddNewData()

	final static int                     iRowCount=3;
	private int                          iTimeSpanSeconds;
	private int                          minRange;
	private Date                         tPlotStartSec;
	private Date                         tPlotEndSec = new Date(0);
	private String                       station;
	private String []                    channel;
	private String                       network;
	private String                       location;
	
	private int []                       iCountsPer = {1, 1, 1};
	private int []                       iCount = {0, 0, 0};
	private Date []                      timeLastSum = {null, null, null};
  private Date []                      midTime = {null, null, null};
  private double []                    minVal = {0, 0, 0};
  private double []                    maxVal = {0, 0, 0};
  private int []                       iMin = {0, 0, 0};
  private int                          minClockQuality=100;

	private TimeSeriesCollection         dataset;
	private JFreeChart                   chart;
	private TimeZone                     timeZone         = TimeZone
	                                                          .getTimeZone("GMT");
	private DateAxis                     date_axis;
	private NumberAxis                   rangeAxis;

} // class TimeChartPlotter
