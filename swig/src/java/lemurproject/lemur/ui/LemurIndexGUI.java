/*==========================================================================
 *
 *  Original source copyright (c) 2004, University of Massachusetts.
 *  See copyright.umass for details.
 *  Modifications copyright (c) 2004, Carnegie Mellon University.
 *  See copyright.cmu for details.
 *
 *==========================================================================
 */

package lemurproject.lemur.ui;

import javax.swing.*;
import javax.swing.table.*;
import javax.swing.event.*;
import javax.swing.border.*;
import java.awt.*;
import java.awt.event.*;
import java.io.*;
import java.util.*;
import java.util.prefs.Preferences;

import lemurproject.lemur.*;

/**
   User interface for building Lemur indexes.
   @author David Fisher
   @version 1.0
*/
/** tnt - modified layout, add JNI, add LemurIndriIndex, add option for 
 *  incremental indexing,additional error checks
 *  
 *  mjh - 5/26/06 - modified various defaults, HCI issues
 */

public class LemurIndexGUI extends JPanel implements ActionListener,
                                                     ItemListener, CaretListener
{
    private final static String SLASH = System.getProperty("file.separator");
    private final static String DMNAME = "dm";

    /** Status line messages visible for all tabs. */
    private JLabel status, arstem;
    /** Messages pane for prog output */
    private JTextArea messages;
    /** Top level container */
    private JTabbedPane tabbedPane;
    /** Boolean choices */
    private JCheckBox countStops, doDM, doStem, doRecurse, doFilter, doStop, doAcro;
    /** what number DM this is for incremental index */
    private int DMnum;
    /** Multiple choices */
    private JComboBox indexTypeBox, dmTypeBox, stemmers, stemFuncs,languageBox,
        memoryLim, docFormat;
    /** Action Buttons */
    private JButton browse, cfbrowse, dmbrowse, stemBrowse, stopBrowse,
        acroBrowse, cfRemove, go, stop;
    /** input/browseable filename */
    private JTextField iname, stopwordlist, acroList;
    /** filename filter */
    private JTextField filterString;
    /** browse multiple filenames */
    private JList collectionFiles;
    /** hold the file names */
    private DefaultListModel cfModel;
    /** separate thread for blinking the status  */
    private Thread bl;
    /** File chooser starting in user's home directory */
    private final JFileChooser fc =
        new JFileChooser(System.getProperties().getProperty("user.home"));
    /** data directory paths */
    private String stopwords, bindir;
    /** Help file for the application */
    private final static String helpFile = "properties/IndexGUI.html";
    /** The little lemur icon */
    private final static String iconFile = "properties/lemur_head_32.gif";
    /** The big lemur logo with L */
    private final static String logoFile = "properties/lemur_logo.gif";
    // the full Lemur
    //    final static String logoFile = "properties/lemur_logo_100.gif";
    
    private final static String defaultStopwordFile="properties" + SLASH + "stoplist.dft";
    
    /** Lemur index types */
    private final static String[] indTypes = {"KeyfileIncIndex", "LemurIndriIndex", "InvFPIndex", "InvIndex"};
    /** Lemur document manager types */
    private final static String[] mgrTypes = {"Element", "Keyfile", "Flatfile"};
    /** Lemur document formats */
    private final static String [] formats = {"trec", "web", "reuters"};
    /** Lemur document formats */
    private final static String [] languages = {"English", "Chinese", "Arabic"};
    /** Memory limit choices */
    private final static String [] lims =
    {
        "  64 MB", "  96 MB", " 128 MB",
        " 256 MB", " 512 MB", "1024 MB"};
    /** Lemur stemmer types */
    private final static String[] sTypes = {"Krovetz", "Porter"};
    /** Arabic stemmer functions */
    private final static String[] sFuncs =
    {
        "arabic_light10_stop",
        "arabic_light10", "arabic_norm2",
        "arabic_norm2_stop"};

    /** MenuBar */
    private JMenuBar menuBar;
    /** Menus */
    private JMenu fileMenu, helpMenu;
    /** Menu Items */
    private JMenuItem fOpen, fSave, fQuit, hHelp, hAbout;
    /** The Lemur Icon. */
    private ImageIcon lemurIcon;
    /** About the indexer. */
    private final static String aboutText = "Lemur Indexer UI 1.0\n" +
        "Copyright (c) 2004 University of Massachusetts\n" +
        "http://www.cs.cmu.edu/~lemur";
    /** Index build parameter file name */
    private String paramFileName;
    /** Index build dataFiles parameter file name */
    private String dataFilesName;
    /** List of data files (expanded) */
    private String fileList;
    /** Thread for building index */
    JBuildIndex builder;

    /** Frame for help window */
    private JFrame helpFrame, parent;

    /** Get the ball rolling. */
    public LemurIndexGUI (JFrame f)
    {
        super(new BorderLayout());
        parent = f;
        lemurIcon = createImageIcon(iconFile);
        ImageIcon icon = null;  // no icon on tabs

        // initialize help
        makeHelp();

        // body of application
        tabbedPane = new JTabbedPane();
        JPanel panel1 = makePanel("Index");
        panel1.setPreferredSize(new Dimension(600,150));
        indexTypeBox = new JComboBox(indTypes);
        indexTypeBox.setSelectedIndex(1);
        indexTypeBox.setToolTipText("Select the index type");
        indexTypeBox.addActionListener(this);
        browse = new JButton("Browse...");
        browse.addActionListener(this);
        browse.setToolTipText("Browse to a directory and enter a basename " +
                              "for the index");
        iname = new JTextField("", 25);
        iname.setToolTipText("Enter a basename for the index or browse to " +
                             "a directory");
        iname.addCaretListener(this);

        JPanel p;
        Spring pad = Spring.constant(5);
        SpringLayout spring = (SpringLayout)panel1.getLayout();
        addLabeledRow(panel1, pad, "Index Type:  ",indexTypeBox);
        addLabeledRow(panel1, Spring.sum(spring.getConstraint(SpringLayout.SOUTH, indexTypeBox),pad), "Index Name: ", iname, browse);

        doDM = new JCheckBox("Build a Document Manager for this index", true);
        doDM.setEnabled(false);
        doDM.setToolTipText("Build a document manager to retrieve original text for this index");
        doDM.addItemListener(this);
        dmTypeBox = new JComboBox(mgrTypes);
        dmTypeBox.setEnabled(false);
        dmTypeBox.setToolTipText("Select document manager type");

        //add DM options
        panel1.add(doDM);
        panel1.add(dmTypeBox);
        spring.getConstraints(doDM).setX(spring.getConstraint(SpringLayout.WEST,iname));
        spring.putConstraint(SpringLayout.NORTH, doDM, pad, SpringLayout.SOUTH, iname);
        addLabeledRow(panel1, spring.getConstraint(SpringLayout.WEST,doDM),
                      Spring.sum(spring.getConstraint(SpringLayout.SOUTH, doDM), pad),
                      "Document Manager Type:", dmTypeBox);

        cfModel = new DefaultListModel();
        collectionFiles = new JList(cfModel);

        collectionFiles.setSelectionMode(ListSelectionModel.MULTIPLE_INTERVAL_SELECTION);
        String fill = "12345678901234567890123456789012345678901234";
        collectionFiles.setPrototypeCellValue(fill);
        collectionFiles.setVisibleRowCount(5);
        collectionFiles.setToolTipText("Browse to a directory and select " +
                                       "input files or directories.");
        JScrollPane listScrollPane = new JScrollPane(collectionFiles);

        cfbrowse = new JButton("Browse...");
        cfbrowse.addActionListener(this);
        cfbrowse.setToolTipText("Browse to a directory and select input " +
                                "files or directories.");

        cfRemove = new JButton("Remove");
        cfRemove.addActionListener(this);
        cfRemove.setToolTipText("Remove selected files from the list.");

        JPanel p2 = makePanel("Documents");
        p2.setPreferredSize(new Dimension(600,220));
        SpringLayout spring2 = (SpringLayout) p2.getLayout();
        addLabeledRow(p2, pad, "<html>Files/Directories <br>to Index: </html>", listScrollPane);
        p2.add(cfbrowse);
        p2.add(cfRemove);
        SpringLayout.Constraints cons = spring2.getConstraints(listScrollPane);
        SpringLayout.Constraints brocons = spring2.getConstraints(cfbrowse);
        brocons.setX(Spring.sum(cons.getConstraint(SpringLayout.EAST),pad));
        brocons.setConstraint(SpringLayout.SOUTH,cons.getConstraint(SpringLayout.SOUTH));
        spring2.getConstraints(cfRemove).setX(brocons.getX());
        spring2.getConstraints(cfRemove).setConstraint(SpringLayout.SOUTH,
                                                       Spring.sum(brocons.getConstraint(SpringLayout.NORTH),Spring.minus(pad)));


        doRecurse = new JCheckBox("include files in sub-directories");
        doRecurse.setToolTipText("<html>When checked and a directory is in the <br>" +
                                 "data files list, recursively add all<br> " +
                                 "data files in that directory and all of <br>" +
                                 " its subbdirectories into the data files list. </html>");

        filterString = new JTextField("", 7);
        filterString.setEnabled(false);
        filterString.setToolTipText("Specify a filename filter, eg *.sgml");
        doFilter = new JCheckBox("index only files with this extension (ie *.dat):");
        doFilter.addItemListener(this);
        p2.add(doFilter);
        p2.add(filterString);
        p2.add(doRecurse);
        spring2.getConstraints(doFilter).setX(cons.getX());
        spring2.getConstraints(doFilter).setY(Spring.sum(cons.getConstraint(SpringLayout.SOUTH),pad));
        spring2.putConstraint(SpringLayout.WEST, filterString, pad, SpringLayout.EAST, doFilter);
        spring2.getConstraints(filterString).setY(spring2.getConstraints(doFilter).getY());
        spring2.getConstraints(doRecurse).setX(cons.getX());
        spring2.getConstraints(doRecurse).setY(Spring.sum(spring2.getConstraint(SpringLayout.SOUTH, doFilter),pad));
        docFormat = new JComboBox(formats);
        docFormat.setToolTipText("Select format of input files");
        docFormat.addActionListener(this);
        addLabeledRow(p2, Spring.sum(spring2.getConstraint(SpringLayout.SOUTH,doRecurse),pad),
                      "Document Format: ", docFormat);

        JPanel p3 = makePanel("Other");
        p3.setPreferredSize(new Dimension(600,95));
        spring = (SpringLayout)p3.getLayout();
        memoryLim = new JComboBox(lims);
        memoryLim.setToolTipText("How much memory to use while indexing. " +
                                 "A rule of thumb is no more than 3/4 of " +
                                 "your physical memory");
        memoryLim.setSelectedIndex(2); // 128 MB

        languageBox = new JComboBox(languages);
        languageBox.addActionListener(this);
        languageBox.setToolTipText("What language your documents are in.");

        addLabeledRow(p3, pad, "Language:    ", languageBox);
        addLabeledRow(p3, Spring.sum(spring.getConstraint(SpringLayout.SOUTH,languageBox),pad),
                      "Memory Use:", memoryLim);

        JPanel bigpanel = new JPanel(new FlowLayout(FlowLayout.CENTER));
        bigpanel.setPreferredSize(new Dimension(600,500));
        bigpanel.add(panel1);
        bigpanel.add(p2);
        bigpanel.add(p3);
        tabbedPane.addTab("Build Options", icon, bigpanel, "Index Options");

        JPanel panel2 = makePanel("Term Processing");
        spring = (SpringLayout)panel2.getLayout();
        doStop = new JCheckBox("Omit stopwords");
        doStop.addItemListener(this);
        panel2.add(doStop);
        countStops = new JCheckBox("Include stopwords in document length count", false);
        countStops.setToolTipText("<html>Whether or not to include words on the <i>stopwords list</i><br> when calculating document length</html>");
        spring.putConstraint(SpringLayout.WEST, doStop, pad, SpringLayout.WEST, panel2);
        spring.putConstraint(SpringLayout.NORTH, doStop, pad, SpringLayout.NORTH, panel2);

        stopwordlist = new JTextField("",25);
        stopwordlist.setText(System.getProperty("user.dir") + SLASH + defaultStopwordFile);
        stopwordlist.setToolTipText("Enter a stopword list or browse to select.");

        stopBrowse = new JButton("Browse...");
        stopBrowse.addActionListener(this);
        stopBrowse.setToolTipText("Browse to a directory and select a stoplist.");
        Spring indent = Spring.constant(25);
        addLabeledRow(panel2,indent, Spring.sum(spring.getConstraint(SpringLayout.SOUTH,doStop),pad),
                      "Stopword List: ", stopwordlist, stopBrowse);

        panel2.add(countStops);
        spring.putConstraint(SpringLayout.WEST,countStops, indent, SpringLayout.WEST, panel2);
        spring.putConstraint(SpringLayout.NORTH,countStops, pad, SpringLayout.SOUTH,stopwordlist);
        countStops.setEnabled(false);
        stopwordlist.setEnabled(false);
        stopBrowse.setEnabled(false);

        doAcro = new JCheckBox("Recognize acronyms");
        doAcro.addItemListener(this);
        panel2.add(doAcro);
        acroList = new JTextField("", 25);
        acroList.setToolTipText("Enter an acronym list or browse to select. ");
        acroBrowse = new JButton("Browse...");
        acroBrowse.addActionListener(this);
        acroBrowse.setToolTipText("Browse to a directory and select an " +
                                  "acronym file.");
        spring.putConstraint(SpringLayout.WEST, doAcro, pad, SpringLayout.WEST, panel2);
        spring.putConstraint(SpringLayout.NORTH, doAcro, indent, SpringLayout.SOUTH, countStops);
        addLabeledRow(panel2, indent, Spring.sum(spring.getConstraint(SpringLayout.SOUTH, doAcro),pad),
                      "Acronym List:", acroList, acroBrowse);
        acroList.setEnabled(false);
        acroBrowse.setEnabled(false);

        doStem = new JCheckBox("Stem words");
        doStem.addItemListener(this);
        doStem.setToolTipText("<html>Select to enable stemming (conflation <br>" +
                              "of morphological variants) for this index</html>");
        panel2.add(doStem);
        spring.putConstraint(SpringLayout.WEST, doStem, pad, SpringLayout.WEST, panel2);
        spring.putConstraint(SpringLayout.NORTH, doStem, indent, SpringLayout.SOUTH, acroList);

        stemmers = new JComboBox(sTypes);
        stemmers.setToolTipText("Select stemming algorithm.");
        stemmers.addActionListener(this);
        stemmers.setEnabled(false);

        stemFuncs = new JComboBox(sFuncs);
        String s = (String)stemmers.getSelectedItem();
        stemFuncs.setVisible(false);
        stemFuncs.setToolTipText("Select arabic stemming function.");

        arstem = new JLabel("Arabic stemmer function: ");
        arstem.setVisible(false);
        addLabeledRow(panel2, indent, Spring.sum(spring.getConstraint(SpringLayout.SOUTH, doStem),pad),
                      "Stemmer: ", stemmers);

        panel2.add(arstem);
        panel2.add(stemFuncs);
        spring.putConstraint(SpringLayout.WEST, arstem, indent, SpringLayout.WEST, panel2);
        spring.putConstraint(SpringLayout.NORTH, arstem, pad, SpringLayout.SOUTH, stemmers);
        spring.getConstraints(stemFuncs).setY(spring.getConstraint(SpringLayout.NORTH,arstem));
        spring.putConstraint(SpringLayout.WEST, stemFuncs, pad, SpringLayout.EAST, arstem);

        tabbedPane.addTab("Parsing", icon, panel2, "Data Handling Options");

        JPanel panel4 = new JPanel(new BorderLayout());
        panel4.setBorder(BorderFactory.createTitledBorder("Program Outputs"));
        messages = new JTextArea(10,40);
        messages.setEditable(false);

        JScrollPane messageScrollPane = new JScrollPane(messages, JScrollPane.VERTICAL_SCROLLBAR_ALWAYS,
                                                        JScrollPane.HORIZONTAL_SCROLLBAR_AS_NEEDED);
        panel4.add(messageScrollPane, BorderLayout.CENTER);
        tabbedPane.addTab("Status", icon, panel4, "Status Messages");

        JPanel buttons = new JPanel();
        buttons.setPreferredSize(new Dimension(500,75));
        go = new JButton("Build Index");
        go.setEnabled(false);
        go.addActionListener(this);
        stop = new JButton("Quit");
        stop.addActionListener(this);
        buttons.add(go);
        buttons.add(stop);
        status = new JLabel("Ready...", lemurIcon, JLabel.LEFT);
        status.setBorder(new BevelBorder(BevelBorder.LOWERED));
        add(tabbedPane, BorderLayout.NORTH);
        add(buttons, BorderLayout.CENTER);
        add(status, BorderLayout.SOUTH);

        builder = new JBuildIndex(this, messages);
        builder.start();
    }
    // gui helper functions.
    private static void setFrameIcon(JFrame fr, String file) 
    {
        Toolkit tk = fr.getToolkit();
        Image img = tk.getImage(file);
        fr.setIconImage(img);
    }

    /** Create the applications menu bar.
        @return the JMenuBar.
    */
    private JMenuBar makeMenuBar()
    {
        // menu
        menuBar = new JMenuBar();
        fileMenu = new JMenu("File");
        helpMenu = new JMenu("Help");
        menuBar.add(fileMenu);
        //  menuBar.add(Box.createHorizontalGlue());
        menuBar.add(helpMenu);

        fOpen = makeMenuItem("Open");
        // leave it off of the menu until it has something to do.
        //  fileMenu.add(fOpen);

        fSave = makeMenuItem("Save");
        fSave.setEnabled(false);
        fileMenu.add(fSave);
        fQuit = makeMenuItem("Quit");
        fileMenu.add(fQuit);
        hHelp = makeMenuItem("Help");
        helpMenu.add(hHelp);
        hAbout = makeMenuItem("About");
        helpMenu.add(hAbout);
        return menuBar;
    }
    /** Creates a JMenuItem with this as its ActionListener.
        @param label the label for the item.
        @return the created menu item.
    */
    private JMenuItem makeMenuItem(String label)
    {
        JMenuItem item  = new JMenuItem(label);
        item.addActionListener(this);
        return item;
    }

    /** Create a JPanel with BorderLayout.
        @return the Jpanel.
    */
    private JPanel makePanel(String title)
    {
        JPanel panel = new JPanel(new SpringLayout());
        TitledBorder border = BorderFactory.createTitledBorder(title);
        border.setTitleJustification(TitledBorder.LEFT);
        panel.setBorder(border);
        return panel;
    }
    /** Create a panel with two elements.
        @param label the String to use to make a label for the first element.
        @param elt  the second element.
        @return the Jpanel.
    */
    private JPanel makeLabeledComponent(String label, JComponent elt)
    {
        JPanel panel = new JPanel();
        JLabel lab = new JLabel(label);
        panel.add(lab);
        panel.add(elt);
        return panel;
    }
    /** Create a panel with three elements.
        @param label the String to use to make a label for the first element.
        @param elt1  the second element.
        @param elt2  the third element.
        @return the Jpanel.
    */
    private JPanel makeLabeledComponent(String label,
                                        JComponent elt1,
                                        JComponent elt2)
    {
        JPanel panel = new JPanel();
        JLabel lab = new JLabel(label);
        panel.add(lab);
        panel.add(elt1);
        panel.add(elt2);
        return panel;
    }

    /** Add a row with 2 elements to a JPanel using SpringLayout
        @param panel the panel
        @param height the height of this row
        @param label the String to use to make a label for the first element.
        @param elt1  the second element.
        @param elt2  the third element.
    */
    private void addLabeledRow(JPanel panel, Spring height,
                               String label,
                               JComponent elt1,
                               JComponent elt2)
    {
        addLabeledRow(panel, Spring.constant(0), height, label, elt1, elt2);
    }

    /** Add a row with 2 elements to a JPanel using SpringLayout
        @param panel the panel
        @param lead lead the row with this much space
        @param height the height of this row
        @param label the String to use to make a label for the first element.
        @param elt1  the second element.
        @param elt2  the third element.
    */
    private void addLabeledRow(JPanel panel, Spring lead, Spring height,
                               String label,
                               JComponent elt1,
                               JComponent elt2)
    {
        JLabel lab = new JLabel(label);
        panel.add(lab);
        panel.add(elt1);
        panel.add(elt2);
        Spring pad = Spring.constant(5);
        SpringLayout layout = (SpringLayout)panel.getLayout();
        SpringLayout.Constraints cons = layout.getConstraints(lab);
        cons.setX(Spring.sum(pad, lead));
        cons.setY(height);
        Spring prev = cons.getConstraint(SpringLayout.EAST);
        cons = layout.getConstraints(elt1);
        cons.setX(Spring.sum(prev,pad));
        cons.setY(height);
        prev = cons.getConstraint(SpringLayout.EAST);
        cons = layout.getConstraints(elt2);
        cons.setX(Spring.sum(prev,pad));
        cons.setY(height);
    }

    /** Add a row with 1 element to a JPanel using SpringLayout
        @param panel the panel
        @param height the height of this row
        @param label the String to use to make a label for the first element.
        @param elt1  the element.
    */
    private void addLabeledRow(JPanel panel, Spring height,
                               String label,
                               JComponent elt1)
    {

        addLabeledRow(panel, Spring.constant(0), height, label, elt1);
    }
    /** Add a row with 1 element to a JPanel using SpringLayout
        @param panel the panel
        @param lead lead the row with this space
        @param height the height of this row
        @param label the String to use to make a label for the first element.
        @param elt1  the element.
    */
    private void addLabeledRow(JPanel panel,
                               Spring lead,
                               Spring height,
                               String label,
                               JComponent elt1)
    {
        JLabel lab = new JLabel(label);
        panel.add(lab);
        panel.add(elt1);
        SpringLayout layout = (SpringLayout)panel.getLayout();
        Spring pad = Spring.constant(5);
        layout.putConstraint(SpringLayout.WEST, lab, Spring.sum(lead, pad),
                             SpringLayout.WEST, panel);
        layout.putConstraint(SpringLayout.WEST, elt1, pad,
                             SpringLayout.EAST, lab);
        layout.getConstraints(lab).setY(height);
        layout.getConstraints(elt1).setY(height);
    }

    /** Returns an ImageIcon, or null if the path was invalid.
        @param path the image file to load.
        @return an ImageIcon, or null if the path was invalid.
    */
    protected static ImageIcon createImageIcon(String path)
    {
        java.net.URL imgURL = LemurIndexGUI.class.getResource(path);
        if (imgURL != null)
            {
                return new ImageIcon(imgURL);
            }
        else
            {
                return null;
            }
    }

    //Listeners
    /** Omnibus for responding to user actions. */
    public void actionPerformed(ActionEvent e)
    {
        Object source = e.getSource();
        if (source == browse)
            {
                fc.setFileSelectionMode(JFileChooser.FILES_AND_DIRECTORIES);
                int returnVal = fc.showOpenDialog(this);
                if (returnVal == JFileChooser.APPROVE_OPTION)
                    {
                        File file = fc.getSelectedFile();
                        iname.setText(file.getAbsolutePath());
                    }
                fc.setFileSelectionMode(JFileChooser.FILES_ONLY);
            }
        else if (source == cfbrowse)
            {
                // pick file or directory
                fc.setFileSelectionMode(JFileChooser.FILES_AND_DIRECTORIES);
                // pick multiple at the same time
                fc.setMultiSelectionEnabled(true);
                // final because the inner class uses it.
                if (doFilter.isSelected())
                    {
                        final String regexp = filterString.getText();
                        if (regexp.length() > 0)
                            {
                                final String filtString = encodeRegexp(regexp);
                                javax.swing.filechooser.FileFilter filt =
                                    new javax.swing.filechooser.FileFilter()
                                    {
                                        public boolean accept(File f)
                                        {
                                            if (f.isDirectory())
                                                {
                                                    return true;
                                                }
                                            String name = f.getName();
                                            return name.matches(filtString);
                                        }
                                        public String getDescription()
                                        {
                                            return regexp + " files";
                                        }
                                    };
                                fc.addChoosableFileFilter(filt);
                            }
                    }
                int returnVal = fc.showOpenDialog(this);
                if (returnVal == JFileChooser.APPROVE_OPTION)
                    {
                        File [] files = fc.getSelectedFiles();
                        for (int i = 0; i < files.length; i++)
                            cfModel.addElement(files[i].getAbsolutePath());
                    }
                fc.setMultiSelectionEnabled(false);
                fc.setFileSelectionMode(JFileChooser.FILES_ONLY);
                // remove the filter.
                fc.resetChoosableFileFilters();
            }
        else if (source == stopBrowse)
            {
                int returnVal = fc.showOpenDialog(this);
                if (returnVal == JFileChooser.APPROVE_OPTION)
                    {
                        File file = fc.getSelectedFile();
                        stopwordlist.setText(file.getAbsolutePath());
                        stopwords = stopwordlist.getText();
                    }
            }
        else if (source == acroBrowse)
            {
                int returnVal = fc.showOpenDialog(this);
                if (returnVal == JFileChooser.APPROVE_OPTION)
                    {
                        File file = fc.getSelectedFile();
                        acroList.setText(file.getAbsolutePath());
                    }
            }
        else if (source == indexTypeBox)
            {
                String type = (String)indexTypeBox.getSelectedItem();
                boolean supported = !type.equals("LemurIndriIndex");
                doAcro.setEnabled(supported);
                if (!supported)
                    {
                        doAcro.setSelected(false);
                        doDM.setSelected(false);
                    }
                acroList.setEnabled(supported);
                acroBrowse.setEnabled(supported);

                doDM.setEnabled(supported);
                dmTypeBox.setEnabled(supported);
            }
        else if (source == languageBox)
            {
                String lang = (String)languageBox.getSelectedItem();
                stemmers.removeAllItems();
                docFormat.removeAllItems();
                //repopulate
                if (lang.equals("English"))
                    {
                        docFormat.addItem("trec");
                        docFormat.addItem("web");
                        docFormat.addItem("reuters");
                        stemmers.addItem("Krovetz");
                        stemmers.addItem("Porter");
                        doStem.setEnabled(true);
                        stemmers.setEnabled(doStem.isSelected());
                        stemFuncs.setVisible(false);
                        arstem.setVisible(false);
                    }
                else if (lang.equals("Arabic"))
                    {
                        docFormat.addItem("arabic");
                        stemmers.addItem("arabic");
                        doStem.setEnabled(true);
                        stemmers.setEnabled(doStem.isSelected());
                        stemFuncs.setVisible(true);
                        arstem.setVisible(true);
                        stemFuncs.setEnabled(doStem.isSelected());
                    }
                else if (lang.equals("Chinese"))
                    {
                        docFormat.addItem("chinese");
                        docFormat.addItem("chinese char");
                        doStem.setEnabled(false);
                        stemmers.setEnabled(false);
                        stemFuncs.setVisible(false);
                        arstem.setVisible(false);
                    }
            }
        else if (source == stemmers)
            {
            }
        else if (source == go)
            {
                // show parameters and build index.
                // sanity check first.
                if (! safeToBuildIndex())
                    {
                        // need more completed messages.
                        status.setText("Unable to build " + iname.getText());
                        return;
                    }
                // flip to status tab
                tabbedPane.setSelectedIndex(2);
                // start a new thread to run in so messages will be updated.
                Runnable r = new Runnable()
                    {
                        public void run()
                        {
                            buildIndex();
                            ensureMessagesVisible();
                        }
                    };
                Thread t = new Thread(r);
                t.start();
            }
        else if (source == stop)
            {
                System.exit(0);
            }
        else if (source == fQuit)
            {
                System.exit(0);
            }
        else if (source == cfRemove)
            {
                Object [] selected = collectionFiles.getSelectedValues();
                for (int i = 0; i < selected.length; i++)
                    {
                        cfModel.removeElement(selected[i]);
                    }
            }
        else if (source == fOpen)
            {
                // to be determined...
                status.setText("Open not implemented.");
                return;
            }
        else if (source == fSave)
            {
                // to be determined...
                boolean written = writeParamFiles();
                if (written)
                    status.setText("Saved: " + paramFileName);
                else
                    status.setText("Unable to save: " + paramFileName);
            }
        else if (source == hHelp)
            {
                // pop up a help dialog
                helpFrame.setVisible(true);
            }
        else if (source == hAbout)
            {
                JDialog abtDialog = new JDialog(parent, "Indexing GUI for the Lemur Toolkit", true);
                abtDialog.getContentPane().setLayout(new FlowLayout());
                //      ImageIcon logo = new ImageIcon(logoFile);
                ImageIcon logo = createImageIcon(logoFile);
                JLabel label = new JLabel("<html><font face=arial size=3 color=#000000><center><h4>Lemur Toolkit GUI</h4>(using Lemur version 4.0)<BR>June 2005<br><br>The Lemur Toolkit was developed <BR>in collaboration between the <br><BR><b>Language Technologies Institute<BR>Carnegie Mellon Univeristy<BR></b>Pittsburgh, PA 15213<br><BR>and<BR><br><b>Center for Intelligent Information Retrieval<BR>University of Massachusetts<BR></b>Amherst, MA 01003<br><BR><BR></font><font face=arial size=3 color=#000000>For more information, visit <BR>www.lemurproject.org</font><html>", logo, JLabel.CENTER);
                label.setVerticalTextPosition(JLabel.BOTTOM);
                label.setHorizontalTextPosition(JLabel.CENTER);

                abtDialog.getContentPane().add(label);    
                abtDialog.getContentPane().setBackground(Color.white);
                abtDialog.setResizable(false);
                abtDialog.setSize(new Dimension(300,400));
                abtDialog.show();
            }
        // at least one datafile and a name entered.
        boolean enabled = (cfModel.getSize() > 0 &&
                           iname.getText().length() > 0);
        go.setEnabled(enabled);
        // fSave.setEnabled(enabled);
    }

    /** Listens to the check boxes. */
    public void itemStateChanged(ItemEvent e)
    {
        Object source = e.getItemSelectable();
        boolean change = (e.getStateChange() == ItemEvent.SELECTED);
        if (source == doDM)
            {
                dmTypeBox.setEnabled(change);
            }
        else if (source == doStem)
            {
                String s = (String)stemmers.getSelectedItem();
                stemFuncs.setEnabled(change && s.equals("arabic"));
                stemmers.setEnabled(change);
            }
        else if (source == doFilter)
            {
                filterString.setEnabled(change);
            }
        else if (source == doStop)
            {
                stopwordlist.setEnabled(change);
                stopBrowse.setEnabled(change);
                countStops.setEnabled(change);
            }
        else if (source == doAcro)
            {
                acroList.setEnabled(change);
                acroBrowse.setEnabled(change);
            }
    }

    /** Listens to the index name text field. This enables updating the
        state of the BuildIndex button when the user types in the name
        of the index directly.
    */
    public void caretUpdate(CaretEvent event) {
        boolean enabled = (cfModel.getSize() > 0 &&
                           iname.getText().length() > 0);
        go.setEnabled(enabled);
        fSave.setEnabled(enabled);
    }


    /** Create the frame that shows the help file and render the html.
        Saves time when the user opens the help.
    */
    private void makeHelp() {
        java.net.URL helpURL = LemurIndexGUI.class.getResource(helpFile);
        JTextPane help = new JTextPane();
        try {
            help.setPage(helpURL);
        } catch (IOException ex) {
            //showException(ex);
            help.setText("Help file unavailable.");
        }

        //Create and set up the window.
        helpFrame = new JFrame("LEMUR Index Builder Help");
        help.setPreferredSize(new Dimension(650, 400));
        help.setEditable(false);
        help.addHyperlinkListener(new DocLinkListener());
        JScrollPane scroller =
            new JScrollPane(help,
                            JScrollPane.VERTICAL_SCROLLBAR_ALWAYS,
                            JScrollPane.HORIZONTAL_SCROLLBAR_ALWAYS);
        scroller.setPreferredSize(new Dimension(700, 450));
        JPanel p = new JPanel();
        p.setOpaque(true);
        p.add(scroller);
        helpFrame.getContentPane().add(p, BorderLayout.CENTER);
        helpFrame.setDefaultCloseOperation(JFrame.HIDE_ON_CLOSE);
        helpFrame.setIconImage(createImageIcon(iconFile).getImage());
        //setFrameIcon(helpFrame, iconFile);
        helpFrame.pack();
    }


    /**
     * Create the GUI and show it.  For thread safety,
     * this method should be invoked from the
     * event-dispatching thread.
     */
    private static void createAndShowGUI()
    {
        //Make sure we have nice window decorations.
        JFrame.setDefaultLookAndFeelDecorated(true);
        // For system look and feel

        //Create and set up the window.
        JFrame frame = new JFrame("LEMUR Index Builder");
        frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frame.setIconImage(createImageIcon(iconFile).getImage());
        //Create and set up the content pane.
        LemurIndexGUI newContentPane = new LemurIndexGUI(frame);
        newContentPane.setOpaque(true); //content panes must be opaque
        frame.getContentPane().add(newContentPane, BorderLayout.CENTER);
        frame.setJMenuBar(newContentPane.makeMenuBar());
        //Display the window.
        frame.pack();
        frame.setVisible(true);
    }

    // Utilities
    /** Rewrite a shell filename pattern to a regular expression. <br>
     *  * -&gt; .*<br>
     ? -&gt; .?<br>
     Add ^ to beginning<br>
     Add $ to end<br>
     . -&gt; \.<br>
     @param regexp the filename pattern, eg "*.dat"
     @return a regular expression suitable for use with String.matches(),
     eg "^.*\.dat$"
    */
    private String encodeRegexp(String regexp) {
        // rewrite shell fname pattern to regexp.
        // * -> .*
        // ? -> .?
        // Add ^,$
        // . -> \.
        String retval = "^" + regexp + "$";
        retval = retval.replaceAll("\\.", "\\.");
        retval = retval.replaceAll("\\*", ".*");
        retval = retval.replaceAll("\\?", ".?");
        return retval;
    }

    /** Tests for likely failure scenarios. If the path to the index
        doesn't exist, fail. If the named index already exists, offer
        a choice do either overwrite it or not. If overwrite is selected,
        removes all files of the form indexname.* in the target directory.
        This is potentially dangerous.
        NB: Could be adapted to work with incremental builds.
        @return true if it is safe to build the index otherwise false.
    */
    private boolean safeToBuildIndex()
    {
        // if the given index name is an existing index, strip the extension
        // if iname is not an absolute path, rewrite it to be so.
        File idx = new File(removeExt(iname.getText()));
        String idxPath = idx.getAbsolutePath();
        final String idxName = idx.getName();

        String idxType = encodeIdx();
        String extension = idxType;
        File dir;
        if (!extension.equals("indri"))
            {
                idx = new File(idxPath + "." + extension);

                // if parent directory does not exist (typein error)
                // fail
                dir = idx.getParentFile();
            }
        else
            {
                // indri index is named as a directory.
                idx = new File(idxPath);
                dir = idx.getParentFile();
            }
        if (!dir.exists())
            {
                errorMsg(this,"Unable to build " + idxPath +
                         "Directory " + dir.getAbsolutePath() +
                         " does not exist.");
                return false;
            }

        String dmext = encodeDM();
        File dmfile = new File (idxPath + DMNAME + "." + dmext);
   
        DMnum = 0;
        // check for DM existence
        if ((doDM.isSelected()) && (dmfile.exists())) {
            //check for namedm*.ext
            //not more than 30
            for (int i=1;i<30;i++) {
                File f = new File (idxPath + DMNAME + Integer.toString(i) + "." + dmext);
                if (f.exists())
                    DMnum = i;
            }
            DMnum++;
        }

        if (DMnum > 30) {
            errorMsg(this, "Sorry, each time you add to the KeyfileIncIndex a new document manager is created.\n" +
                     "At this time (being the first version), this number was limited to 30. \nThe only way to change this restriction is to edit the code.");
            return false;
        }
        // if idx exists, either fail or blow it away.
        if (idx.exists())
            {
                boolean delete = false;
                int val;
                if (extension.equals("key"))
                    {
                        String[] choices = {"Add to index", "Overwrite old index", "Cancel"};
                        val = JOptionPane.showOptionDialog(this,
                                                           idx + " exists. New documents can be incrementally added to this index.\n" +
                                                           "Would you like to add new documents or delete the old index?\n" +
                                                           "Choosing \"Overwrite old index\" will DELETE ALL FILES NAMED" +
                                                           idxName + ".*, including any document managers",
                                                           "Index exists",
                                                           JOptionPane.YES_NO_CANCEL_OPTION,
                                                           JOptionPane.QUESTION_MESSAGE,
                                                           null,
                                                           choices,
                                                           choices[0]);
                        if ((val == JOptionPane.CANCEL_OPTION) || (val == JOptionPane.CLOSED_OPTION))
                            return false;
                        if (val == JOptionPane.NO_OPTION)
                            delete = true;
                    } else if (extension.equals("indri")) {
                        String[] choices = {"Add to index", "Overwrite old index", "Cancel"};
                        val = JOptionPane.showOptionDialog(this,
                                                           idx + " exists. New documents can be incrementally added to this index.\n" +
                                                           "Would you like to add new documents or delete the old index?\n" +
                                                           "Choosing \"Overwrite old index\" will DELETE ALL FILES IN THE \"" +
                                                           idxName + "\" directory.",
                                                           "Index exists",
                                                           JOptionPane.YES_NO_CANCEL_OPTION,
                                                           JOptionPane.QUESTION_MESSAGE,
                                                           null,
                                                           choices,
                                                           choices[0]);
                        if ((val == JOptionPane.CANCEL_OPTION) || (val == JOptionPane.CLOSED_OPTION))
                            return false;
                        if (val == JOptionPane.NO_OPTION)
                            delete = true;
                    }
                else
                    {
                        val = JOptionPane.showConfirmDialog(this,
                                                            idx + " exists. Overwrite? \n" +
                                                            "ALL FILES NAMED " + idxName +
                                                            ".* WILL BE DELETED, including any document managers",
                                                            "Overwrite existing index",
                                                            JOptionPane.YES_NO_OPTION,
                                                            JOptionPane.WARNING_MESSAGE);
                        if ((val == JOptionPane.NO_OPTION) || (val == JOptionPane.CLOSED_OPTION))
                            return false;

                        delete = true;
                    }
                // overwrite -- delete all index files.
                // this removes all files of the form indexname.*
   
                if (delete)
                    {
                        if (extension.equals("indri"))
                            {
                                dir = new File(dir.getAbsolutePath() + SLASH + idxName);
                                recurseDelete(dir);
                            }
                        else
                            {
                                deleteFiles(dir, idxName + ".");
                                if ((doDM.isSelected()) && (dmfile.exists())) {
                                    deleteFiles(dir, idxName + DMNAME + ".");
                                    for (int i=1;i<DMnum;i++) {
                                        deleteFiles(dir, idxName + DMNAME + Integer.toString(i) + ".");
                                    }
                                }
                                //reset num for building the next DM
                                DMnum = 0;
                            }
                    }
                //return true;
            } // if exists

        String value;
        String errormsg = "<html>";

        // do more error checking
        if (doFilter.isSelected())
            {
                value = filterString.getText();
                if (value.equals(""))
                    {
                        errormsg += "You wanted to index only files with a certain extension, but did not specify which extension. <Br>";
                        errormsg += "Look under 'Build Options' tab.<br><Br>";
                    }
            }

        if (doStop.isSelected())
            {
                value = stopwordlist.getText();
                if (value.equals(""))
                    errormsg += "Specify a stopword list in order to omit stopwords. Look under 'Parsing' tab options.<br><br>";
            }
        if (doAcro.isSelected())
            {
                value = acroList.getText();
                if (value.equals(""))
                    {
                        errormsg += "Specify a list of acronyms to recognize acronyms. Look under 'Parsing' tab options.<Br><br>";
                    }
            }

        if (errormsg.equals("<html>"))
            return true;
        else
            {
                errormsg += "</html>";
                errorMsg(this, errormsg);
                return false;
            }
    }

    private String removeExt(String name) 
    {
        if (name.endsWith(".ifp"))
            return name.substring(0,name.lastIndexOf(".ifp"));

        if (name.endsWith(".inv"))
            return name.substring(0,name.lastIndexOf(".inv"));

        if (name.endsWith(".key"))
            return name.substring(0,name.lastIndexOf(".key"));
        // or do nothing
        return name;
    }

    private void deleteFiles(File dir, String filter) {
        final String myfil = filter;
        File[] files = dir.listFiles(new FilenameFilter()
            {
                public boolean accept(File dir, String name)
                {
                    return name.startsWith(myfil);
                }

            });

        for (int i = 0; i < files.length; i++)
            {
                File f = files[i];
                messages.append("Deleting: " + f.getAbsolutePath() + "\n");
                f.delete();
            }

    }

    private void recurseDelete(File file)
    {
        File[] contents = file.listFiles();
        for (int i=0;i<contents.length;i++)
            {
                File f = contents[i];
                if (f.isDirectory())
                    recurseDelete(f);
                else
                    {
                        messages.append("Deleting: " + f.getAbsolutePath() + "\n");
                        f.delete();
                    }
            }
        messages.append("Deleting: " + file.getAbsolutePath() + "\n");
        file.delete();
    }

    /** Keeps the message pane showing the last line appended. */
    private void ensureMessagesVisible() {
        int len = messages.getText().length();
        try {
            messages.scrollRectToVisible(messages.modelToView(len));
        } catch (javax.swing.text.BadLocationException ex) {
            // don't care.
        }
    }

    /** Construct the parameter files and build the index.*/
    private void buildIndex()
    {
        Cursor wait = Cursor.getPredefinedCursor(Cursor.WAIT_CURSOR);
        Cursor def = Cursor.getPredefinedCursor(Cursor.DEFAULT_CURSOR);
        setCursor(wait);
        messages.setCursor(wait);
        // if iname is not an absolute path, rewrite it to be so.
        File idx = new File(iname.getText());
        iname.setText(idx.getAbsolutePath());
        String extension = encodeIdx();

        status.setText("Building " + iname.getText() + "...");
        bl = blinker(status.getText(),
                     "Finished building " + iname.getText());
        // construct parameter file
        ensureMessagesVisible();
        boolean written = writeParamFiles();
        if (! written)
            {
                // bad things happening here.
                messages.append("\nUnable to write parameter files.\n");
                ensureMessagesVisible();
                setCursor(def);
                messages.setCursor(def);
                blinking = false;
                bl.interrupt();
                return; // bail
            }
        builder.setParam(paramFileName);
        synchronized(builder)
            {
                try
                    {
                        builder.notify();
                    } catch (Exception e) {
                        errorMsg(this, "An error occurred while building.\n\n" + e.toString());
                    }
            }

        // disable the build button...
        go.setEnabled(false);
        
        // don't let 'em quit easy while it is running.
        fQuit.setEnabled(false);
        stop.setEnabled(false);
    }

    /** Write the parameters file and dataFiles file, echoing their contents
        in the messages window.
        @return true if successful, false if an exception occurs.
    */
    private boolean writeParamFiles() 
    {
        // if iname is not an absolute path, rewrite it to be so.
        File idx = new File(iname.getText());
        iname.setText(idx.getAbsolutePath());
        idx = new File(removeExt(iname.getText()));
        File dir = idx.getParentFile();
        String idxName = idx.getName();
        String dirName = dir.getAbsolutePath();
        String baseName = dirName + File.separator + "build-" + idxName;
        paramFileName =  baseName + ".param";
        dataFilesName = baseName + ".data";
        messages.append(paramFileName + ":\n");
        boolean written = writeParams();
        messages.append("\n" + dataFilesName + ":\n"+ fileList);
        ensureMessagesVisible();
        return written;
    }

    /** Write the lemur parameters to a file.
        @return true if successful, false if an exception occurs.
    */
    private boolean writeParams()
    {
        File f = new File(paramFileName);
        PrintWriter p;
        try
            {
                p = new PrintWriter(new FileWriter(f));
            }
        catch (IOException e)
            {
                // fix this to rethrow?
                showException(e);
                return false;
            }
        p.println("<parameters>");
        messages.append("<parameters>\n");

        String idx = encodeIdx();
        String idxname = removeExt(iname.getText());
        printParam(p, "index", idxname);
        printParam(p, "indexType", idx);
        if (idx.equals("inv"))
            {
                printParam(p, "position", "0");
            }
        if (doDM.isSelected())
            {
                // needs filter to valid values
                if (DMnum == 0)
                    printParam(p, "manager", idxname + DMNAME);
                else
                    printParam(p, "manager", idxname + DMNAME + Integer.toString(DMnum));
                // needs filter to valid values.
                printParam(p, "managerType", encodeDM());
            }
        if (doStem.isSelected())
            {
                String stemmer = (String)stemmers.getSelectedItem();
                printParam(p, "stemmer", stemmer);
                if (stemmer.equals("arabic"))
                    {
                        printParam(p, "arabicStemFunc",
                                   (String) stemFuncs.getSelectedItem());
                    }
            }
        printParam(p, "countStopWords", countStops.isEnabled() ? "true" : "false");
        printParam(p, "memory", encodeMem());
        // stopwords
        if (doStop.isSelected())
            {
                printParam(p, "stopwords", stopwordlist.getText());
            }
        // acronyms
        if (doAcro.isSelected())
            {
                printParam(p, "acronyms", acroList.getText());
            }
        // docFormat
        printParam(p, "docFormat", (String)docFormat.getSelectedItem());
        printParam(p, "dataFiles", dataFilesName );
        p.println("</parameters>");
        messages.append("</parameters>\n");
        p.close();
        return writeDataFiles();
    }

    /** Create the datafiles file and write filenames from the
        input files list.
        @return true if successful, false if an exception occurs.
    */
    private boolean writeDataFiles() {
        // handle directories, recursion, filename patterns
        fileList = "";

        FileFilter filt = null;
        if (doFilter.isSelected())
            {
                final String regexp = filterString.getText();
                if (regexp.length() > 0)
                    {
                        final String filtString = encodeRegexp(regexp);
                        filt = new FileFilter()
                            {
                                public boolean accept(File thisfile)
                                {
                                    String name = thisfile.getName();
                                    return (thisfile.isDirectory() ||
                                            name.matches(filtString));
                                }
                            };
                    }
            }
        File f = new File(dataFilesName);
        PrintWriter p;
        try {
            p = new PrintWriter(new FileWriter(f));
        } catch (IOException e) {
            showException(e);
            return false;
        }

        boolean retval = true;
        Enumeration e = cfModel.elements();
        while (e.hasMoreElements()) {
            String s = (String) e.nextElement();
            File file = new File(s);
            writeDataFiles(p, file, filt);
        }
        p.close();
        return retval;
    }

    /** Write filenames to the datafiles file.
        Append the same line to the messages text area.
        If the File is a directory, iterate over all of the files
        in that directory that satisfy the filter. If recurse into
        subdirectories is selected and the File is a directory,
        invoke recursivly on on all directories within the directory.
        @param p the PrintWriter for the open datafiles file.
        @param file a File (either a file or directory)
        @param f the filename filter to apply.
    */

    private void writeDataFiles(PrintWriter p, File file, FileFilter f) {
        if (file.isDirectory()) {
            // handle directory
            File [] files = file.listFiles(f);
            for (int i = 0; i < files.length; i++) {
                if (files[i].isDirectory()) {
                    if (doRecurse.isSelected()) {
                        writeDataFiles(p, files[i], f);
                    }
                } else {
                    fileList = fileList + files[i].getAbsolutePath() + "\n";
                    p.println(files[i].getAbsolutePath());
                }
            }
        } else {
            fileList = fileList + file.getAbsolutePath() + "\n";
            p.println(file.getAbsolutePath());
        }
    }

    /** Encode the index type as appropriate to use as a
        parameter value.
        @return the encoded type.
    */
    private String encodeIdx()
    {
        String s = (String)indexTypeBox.getSelectedItem();
        String retval;
        if (s.equals("KeyfileIncIndex")) retval = "key";
        else if (s.equals("LemurIndriIndex")) retval = "indri";
        else if (s.equals("InvFPIndex")) retval = "ifp";
        else if (s.equals("InvIndex")) retval = "inv";
        else retval = "unknown";
        return retval;
    }
    /** Encode the document manager type as appropriate to use as a
        parameter value.
        @return the encoded type.
    */
    private String encodeDM() {
        String s = (String)dmTypeBox.getSelectedItem();
        String retval;
        if (s.equals("Keyfile")) retval = "bdm";
        else if (s.equals("Flatfile")) retval = "flat";
        else if (s.equals("Element")) retval = "elem";
        else retval = "unknown";
        return retval;
    }
    /** Encode the memory limit selection into an integer number of bytes.
        @return a String containing the memory limit selection as an integer number of bytes.
    */
    private String encodeMem() {
        String s = ((String)memoryLim.getSelectedItem()).trim();
        int space = s.indexOf(' ');
        return s.substring(0, space) + "000000";
    }

    /** Write a parameter = value; line to the parameter file.
        Append the same line to the messages text area.
        @param p the PrintWriter for the open parameter file.
        @param parm the name of the parameter
        @param val the value of the parameter
    */
    private void printParam(PrintWriter p, String parm, String val) {
        String line = "<" + parm + ">" + val + "</" + parm + ">";
        p.println(line);
        messages.append(line + "\n");
    }

    public void buildDone()
    {
        fQuit.setEnabled(true);
        go.setEnabled(true);
        stop.setEnabled(true);

        blinking = false;
        bl.interrupt();
        setCursor(Cursor.getPredefinedCursor(Cursor.DEFAULT_CURSOR));
        messages.setCursor(Cursor.getPredefinedCursor(Cursor.DEFAULT_CURSOR));

        String ext = encodeIdx();
        File toc;
        if (ext.equals("indri"))
            {
                // nothing to show, can't know, a priori, where the manifest will be.
                status.setText("Finished building " + iname.getText());
                messages.append("Finished building " + iname.getText() + "\n\n");
                ensureMessagesVisible();
                return;
            }
        else
            {
                toc = new File(removeExt(iname.getText()) + "." + ext);
            }
    
        try
            {
                messages.append("Index Stats " + toc.getAbsolutePath() + ":\n");
                BufferedReader buf = new BufferedReader(new FileReader(toc));
                String line;
                while((line = buf.readLine()) != null)
                    {
                        messages.append(line + "\n");
                    }
                buf.close();
                status.setText("Finished building " + iname.getText());
                messages.append("Finished building " + iname.getText() + "\n\n");
                messages.append("The index has been successfully built. You may now either exit the Lemur Indexing GUI and use the index, or, build another index.");
                ensureMessagesVisible();
            }
        catch (IOException ex)
            {
                // unlikely.
                showException(ex);
                buildFailed();
            }
    }

    public void buildFailed()
    {
        fQuit.setEnabled(true);
        stop.setEnabled(true);

        blinking = false;
        bl.interrupt();
        setCursor(Cursor.getPredefinedCursor(Cursor.DEFAULT_CURSOR));
        messages.setCursor(Cursor.getPredefinedCursor(Cursor.DEFAULT_CURSOR));
        status.setText("Build Incomplete");
        messages.append("Build Incomplete");
        ensureMessagesVisible();
    }

    private void errorMsg(Component par, String mesg)
    {
        JOptionPane.showMessageDialog(par, mesg, "Error", JOptionPane.ERROR_MESSAGE);
    }

    /** Format an exception message in the messages text area.
        @param e the exception
    */
    private void showException(Exception e) {
        messages.append("\nERROR:\n");
        StringWriter msg = new StringWriter();
        PrintWriter w = new PrintWriter(msg);
        e.printStackTrace(w);
        w.close();
        messages.append(msg.toString());
        ensureMessagesVisible();
    }

    /** Is the blinker running? */
    private volatile boolean blinking = false;
    /** Make the status line blink while working. */
    private Thread blinker(final String s1, final String s2) {
        Thread blink;
        blink = new Thread(new Runnable() {
                public void run() {
                    String onText = s1;
                    String doneText = s2;
                    String offText = "";
                    int count = 0;
                    try {
                        while (blinking) {
                            Thread.sleep(500);
                            if (count%2 == 0) {
                                status.setText(offText);
                            } else {
                                status.setText(onText);
                            }
                            count++;
                        }
                    } catch (InterruptedException ex) {
                        status.setText(doneText);
                    }
                }
            });
        blinking = true;
        blink.start();
        return blink;
    }
    /** Fire it up.*/
    public static void main(String[] args) {
        try
            {
                UIManager.setLookAndFeel( UIManager.getSystemLookAndFeelClassName() );
            }
        catch (Exception e) { }
        //Schedule a job for the event-dispatching thread:
        //creating and showing this application's GUI.
        javax.swing.SwingUtilities.invokeLater(new Runnable() {
                public void run() {
                    createAndShowGUI();
                }
            });
    }
    /** Simple pop an internalframe for hyperlinks.
        Spawns a new JFrame to hold the individuals.
    */
    class DocLinkListener implements HyperlinkListener {
        JFrame f;
        JDesktopPane deskTop;
        int winCount;
        public DocLinkListener() {
            winCount = 0;
            f = new JFrame("Web Links");
            deskTop = new JDesktopPane();
            deskTop.setPreferredSize(new Dimension(700, 500));
            f.setContentPane(deskTop);
            f.pack();
            f.setIconImage(createImageIcon(iconFile).getImage());
            f.setDefaultCloseOperation(JFrame.HIDE_ON_CLOSE);
        }

        public void hyperlinkUpdate(HyperlinkEvent e) {
            if (e.getEventType() == HyperlinkEvent.EventType.ACTIVATED) {
                java.net.URL link = e.getURL();
                JInternalFrame iframe = new JInternalFrame(e.getDescription(),
                                                           true,
                                                           true,
                                                           true,
                                                           true);
                JTextPane doc = new JTextPane();
                doc.setPreferredSize(new Dimension(500, 350));
                doc.setEditable(false);
                doc.addHyperlinkListener(this);
                JScrollPane scroller =
                    new JScrollPane(doc,
                                    JScrollPane.VERTICAL_SCROLLBAR_ALWAYS,
                                    JScrollPane.HORIZONTAL_SCROLLBAR_ALWAYS);
                scroller.setPreferredSize(new Dimension(600, 400));
                JPanel p = new JPanel();
                p.setOpaque(true);
                p.add(scroller);

                try {doc.setPage(link);
                } catch (IOException ex) {
                    showException(ex);
                    return;
                }
                iframe.getContentPane().add(p, BorderLayout.CENTER);
                iframe.setDefaultCloseOperation(JFrame.DISPOSE_ON_CLOSE);
                iframe.pack();
                //Set the window's location.
                iframe.setLocation(30*winCount, 30*winCount);
                winCount++;
                iframe.setVisible(true);
                deskTop.add(iframe);
                try {
                    iframe.setSelected(true);
                } catch (java.beans.PropertyVetoException v) {
                    // don't care.
                }
                f.setVisible(true);
            }
        }
    }
}
