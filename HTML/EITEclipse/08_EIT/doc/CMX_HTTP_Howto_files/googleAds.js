<!-- 

  // This function displays the ad results. 
  // It must be defined above the script that calls show_ads.js 
  // to guarantee that it is defined when show_ads.js makes the call-back. 

  function google_ad_request_done(google_ads) { 

    // Proceed only if we have ads to display! 
    if (google_ads.length < 1 ) 
      return; 

    // Display ads in a table 
    //document.write("<table cellpadding=\"10\"  align=\"left\" bgcolor=\"#eeeeff\" border=\"1\">"); 
  
    // Print "Ads By Google" -- include link to Google feedback page if available 
    //document.write("<tr><td align=\"left\"><font color=\"#6f6f6f\" size=\"-2\" >"); 
    //if (google_info.feedback_url) { 
    //  document.write("<a href=\"" + google_info.feedback_url + 
    //    "\">Ads by Google</a>"); 
    //} else { 
    //  document.write("Ads By Google"); 
    //} 
    //document.write("</font></td></tr>");   
  
    // For text ads, display each ad in turn. 

    if (google_ads[0].type == 'text') { 
      for(i = 0; i < google_ads.length; ++i) { 
        document.write("<div class=\"googlediv\"><a href=\"" +  google_ads[i].url + "\"target=\"_blank\" class=\"truegoogle\">" +
		  google_ads[i].line1 + "</a><br>" + "<a href=\"" +  google_ads[i].url + "\"target=\"_blank\" class=\"googleadtext\" >" + 
          google_ads[i].line2 + " " + google_ads[i].line3 + "<br>" + "</a>" + 
          "<a href=\"" + google_ads[i].url + "\"target=\"_blank\" class=\"googlesublink\">" + 
          google_ads[i].visible_url + 
          "</a></div>"); 
      } 
    } 

    // For an image ad, display the image; there will be only one . 
    if (google_ads[0].type == 'image') { 
      document.write("<tr><td align=\"center\">" + 
        "<a href=\"" + google_ads[0].url + "\">" + 
        "<img src=\"" + google_ads[0].image_url + 
        "\" height=\"" + google_ads[0].height + 
        "\" width=\"" + google_ads[0].width + 
        "\" border=\"0\"></a></td></tr>"); 
    } 

    // Finish up anything that needs finishing up 
    //document.write ("</table>"); 
  } 

function RandomizeToString ( myArray ) 
{ 
  //This function randomizes a list of words 
  var i = k = myArray.length; 
  var strOut = ""; //"'"; 
  while ( i-- ) 
  { 
     var j = Math.floor( Math.random() * ( i + 1 ) ); 
     var tempi = myArray[i]; 
     var tempj = myArray[j]; 
     myArray[i] = tempj; 
     myArray[j] = tempi; 
   } 
  while ( k-- ) 
   { 
     strOut = strOut + myArray[k] + ", "; 
   } 
   return strOut.substring(0,strOut.length - 2); // + "'"; 
} 

function testIsValidObject(objToTest) { 
        // This function tests to see if a element exists on the web Page 
        if (null == objToTest) { 
                return false; 
        } 
        if ("undefined" == typeof(objToTest) ) { 
                return false; 
        } 
        return true; 
} 

        // These Variables define what will be displayied 
         google_ad_client   = 'ca-penton_html'; 
         google_ad_output   = 'js'; 
         google_max_num_ads = 5; //number of ads 
         google_ad_channel  = '<cfoutput>#adchannel#</cfoutput>'; 
         google_page_url  = window.location.href; 
         google_ad_type   = 'text_image'; 
         google_image_size  = '468x60'; //728x90 , 468x60 , 300x250 , 120x600 
         google_language  = 'en'; 

         //google_encoding  = '[optional: desired output encoding]'; 
          
         //Hint Keywords for the site ... please change them to be what you want for your site, Order is Randomized so different emphasis is placed each view, and the same set of Ads do not get show again and again. 


         GKeywords = new Array("Electronic", "Circuit", "Chip", "Transistor", "Silicon", "Engineering", "Fleets", "Semiconductor", "Amplifiers", "Microprocessor", "Power Source", "Modules", "PLC", "FPGA", "Embedded", "Power", "Communications ICs", "Audio ICs", "Circuits", "Electronic Design", "Boards", "Test Measurement", "Amplifiers", "Modules", "PC/104", "PC104", "PC-104", "Microprocessors", "DSPs", "DLPs", "Microcontrollers", "Single Board Computer"); 
         google_hints = RandomizeToString(GKeywords); 

         google_kw = ''; // no kewords unless its a serach page 
                  
         //If on a search page you want to return ads relevent to search ... 
         //in your search page code please insert HTML that can be found by this java script 
         //For example the HTML: <form id='GoogleSearch' name='GoogleSearch'><input type='hidden' id='Words' name='Words' Value='Search_Terms'></form> 
         // is inserted on a search for 'Search_Terms' 
         if (testIsValidObject(document.GoogleSearch)) { //Sees if The Form 'GoogleSearch' exists 
                if (testIsValidObject(document.GoogleSearch.Words)) { //Sees if The Hidden Input 'Words' exists 
                        // if they exist set the google ad keyword to the Search_Terms 
                        google_kw = unescape(document.GoogleSearch.Words.value).replace("+"," "); 
                        // adjust the hints so Serach_Term takes presidence but the other hints are still there 
                        google_hints = google_kw  + ", " + google_hints; 
                        // Broaden the search for ads so if there is no exact match to the key_words it uses the Hints (synonyms are then applied as well) 
                        google_kw_type = 'broad'; //'broad' or 'exact' 
                } 
        } 
                  
        
         //google_contents  = '[optional: replacement content]'; 
         //Specifies content for ad targeting. If contents are specified, Google will not crawl the page to determine the page content. Instead, the text specified in the contents parameter is used for targeting. Please note this alternate method of targeting is not recommended over the standard method of crawling. The values of the contents parameter can be any HTML or plain text. Contents must be encoded using the default encoding associated with the specified target language. (See Output Encoding for more details on the defaults.) 
         //google_color_line   = '[optional: color of the border surrounding the ads]'; 
         google_safe      = 'high'; 
         //google_adtest    = 'on'; 