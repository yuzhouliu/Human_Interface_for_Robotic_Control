import java.io.*;

/*
 * A tool that converts a byte file to its whitespace-delimited decimal
 * representation.
 * 
 * Copyright (c) 2015 Brandon To, Minh Mai, and Yuzhou Liu
 * This code is licensed under BSD license (see LICENSE.txt for details)
 * 
 * @created February 20, 2016
 */
/****************************************************************************/
public class FileByteToDecConverter
{
    public static void main(String[] args) throws IOException
    {
        String path = "C:\\Users\\brandonto\\cpp-workspace\\"
                + "Human_Interface_for_Robotic_Control\\Controller\\data\\"
                + "playback\\test.hirc";
        String newPath = path.replace(".hirc", "-decimal.hirc");
        FileInputStream fis = new FileInputStream(path);
        FileOutputStream fos = new FileOutputStream(newPath);
        
        int byteOfData;
        while ((byteOfData = fis.read()) != -1)
        {
            String byteString = String.valueOf(byteOfData) + " ";
            fos.write(byteString.getBytes());
        }
        
        fis.close();
        fos.close();
    }
}
