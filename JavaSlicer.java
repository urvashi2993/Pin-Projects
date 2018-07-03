import java.io.*;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Iterator;
import java.util.Scanner;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

public class JavaSlicer {

	@SuppressWarnings("unchecked")
	public static void main(String[] args) {
		
	try{
		@SuppressWarnings("rawtypes")
		ArrayList storeWordList = new ArrayList();
		ArrayList OutputGenerator = new ArrayList();
		
        FileInputStream fstream = new FileInputStream(args[0]);
        // Get the object of DataInputStream
        DataInputStream in = new DataInputStream(fstream);
        @SuppressWarnings("resource")
		BufferedReader br = new BufferedReader(new InputStreamReader(in));
        String strLine;
        //Read File Line By Line
        while ((strLine = br.readLine()) != null) {
            storeWordList.add(strLine);
             
        }
        Collections.reverse(storeWordList);
        String patternString = new String();
        		patternString=args[1];		
        		patternString=patternString.substring(1, 21);
        		OutputGenerator.add(patternString.substring(0,12));
        
        		for (@SuppressWarnings("rawtypes")
        		Iterator iter = storeWordList.iterator(); iter.hasNext();){
        String line = iter.next().toString();
       // System.out.println("\n Line" +line);
        if(line.contains(patternString)){
        	
        	int i=0,count=0;
        	while(i< line.length()){
        		if(line.charAt(i)==','){
        			count++;
        		}
        		if(count==3)
        			break;
        		i++;
        	}
        	
        	if(!Character.isDigit(line.charAt(i+1))){
        		//System.out.println("\n pattern string ="+patternString+"\n"+line.substring(line.indexOf(patternString)+i+3,32));
            	patternString= line.substring(line.indexOf(patternString)+i+3,31);	
            	OutputGenerator.add(patternString);
            	
        	}
        	else{
        	//System.out.println("\n pattern string ="+patternString+"\n"+line.substring(line.indexOf(patternString)+22,36));
        	patternString= line.substring(line.indexOf(patternString)+22,36);
        	OutputGenerator.add(patternString);
        	}
        }
        }
        		Collections.reverse(OutputGenerator);
        		for(Iterator iter = OutputGenerator.iterator(); iter.hasNext();){
        			System.out.println((String)iter.next());
        		}
	}
	catch(Exception e){
		System.out.println("Error :");
	}
	
	


       
	}
}