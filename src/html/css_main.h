#ifndef CSS_MAIN
#define CSS_MAIN 

const char CSS_STYLE[] PROGMEM = R"=====(
      body {
          font-family: "Lato", sans-serif;
      }

      * {
          box-sizing: border-box;
      }
      /*
      .content {
          float: left;
          width: 85%;
          padding: 0 4%;
          overflow: hidden;
          margin-left: 15%; 
          font-size: 28px; 
      }*/
      div.content {
        margin-left: 250px;
        padding: 1px 16px;
        height: 1000px;
      }
      .sidenav {
          margin: 0;
          padding: 0;
          width: 250px;
          background-color: #f1f1f1;
          position: fixed;
          height: 100%;
          overflow: auto;
          background-color: #111;
      }
      .sidenav a {
          padding: 6px 8px 6px 16px;
          text-decoration: none;
          font-size: 25px;
          color: #818181;
          display: block;
      }
      .sidenav a:hover {
        color: #f1f1f1;
      }
      .sidenavitem a{
          clear: both;
          padding: 8px;
          border-bottom: 1px solid #f1f1f1;
      }

      .sidenavitem label{
        padding: 8px;
        color: white;
      }
      .sidenavitem label.textright {
        float: right;
      }  
      .sidenavitem label.textleft {
        float: left;
      }  
      .sidenavitem label.little {
        float: left;
        color: #818181;
        cursor: pointer;
      }  
      .sidenavitem label.little:hover {
        color: #f1f1f1;
      }  
      
      .sidenavitem p{
        padding: 8px;
        color: white;
      }
      .sidenavitem p.textright {
        float: right;
      }  
      .sidenavitem p.textleft {
        float: left;
      }        

      .form-inline {  
        display: flex;
        flex-flow: row wrap;
        align-items: center;

      }
      .form-inline label {
        margin: 5px 10px 5px 0;
        width: 15%;
      }


      .button {
        background-color: #666666;
        border: none;
        color: white;
        padding: 10px 32px;
        text-align: center;
        text-decoration: none;
        display: inline-block;
        font-size: 16px;
        margin: 4px 2px;
        cursor: pointer;
      }
      .button.rounded {
        border-radius: 5px;
      }    
      .button.inline {
        display: flex;
        flex-flow: row wrap;
        align-items: center;
      }         
      .button:hover {
        background-color: #595959;
        color: black;
        transition-duration: 0.4s;
      }
      .input-text {
        margin-top: 0.5%;
        padding:8px;
        display:block;
        border:none;
        border-bottom:1px solid #ccc;
        width:100%
      }
      .input-text.inline {
        vertical-align: middle;
        margin: 5px 5px 10px 0;
        padding: 10px;
        background-color: #fff;
        border: 1px solid #ddd;
        width: 60%
      }
      .color-grey,.color-hover-grey:hover,.color-gray,.color-hover-gray:hover{color:#000!important;background-color:#d3d3d3!important}
      .color-gray-dark,.color-hover-gray-dark:hover{color:#000!important;background-color:#1a1a1a!important}
      .blue,.hover-blue:hover{color:#fff!important;background-color:#2196F3!important}

      .slider {
        margin-top: 0.5%;
        -webkit-appearance: none;
        width: 100%;
        height: 15px;
        border-radius: 5px;  
        background: #d3d3d3;
        outline: none;
        opacity: 0.7;
        -webkit-transition: .2s;
        transition: opacity .2s;
      }
      .slider.inline {
        vertical-align: middle;
        margin: 5px 5px 10px 0;
        padding: 8px;
        border: 1px solid #ddd;
        width:60%
      }

      .slider::-webkit-slider-thumb {
        -webkit-appearance: none;
        appearance: none;
        width: 25px;
        height: 25px;
        border-radius: 50%; 
        background: #4CAF50;
        cursor: pointer;
      }

      .slider::-moz-range-thumb {
        width: 25px;
        height: 25px;
        border-radius: 50%;
        background: #000000;
        cursor: pointer;
      }

      .container,.panel{
        padding:0.01em 16px
        }.panel
        {margin-top:16px;
          margin-bottom:16px
        }


      /* Remove extra left and right margins, due to padding in columns */
      .row {margin: 0 -5px;}

      /* Clear floats after the columns */
      .row:after {
        content: "";
        display: table;
        clear: both;
      }
      /* Float four columns side by side */
      .column {
        margin-top: 15px;
        float: left;
        width: 33%;
        padding: 0 10px;
      }

      .card{
        box-shadow:0 4px 10px 0 rgba(0,0,0,0.2),0 4px 20px 0 rgba(0,0,0,0.19);
        transition: 0.3s;
        border-radius: 8px;          
      }  
      .card:hover {
        box-shadow: 0 8px 20px 0 rgba(0,0,0,0.3),0 4px 20px 0 rgba(0,0,0,0.19);
      }        
      .card label.test{font-size: 35px;}
      .card header{font-size: 35px;}

      label.lblValue { float: left; width: 50%; }
      span.lblValue { margin-left: 50%; display: block; }

      @media screen and (max-width: 1600px) {
        .column {
          width: 50%;
        }    
      }   
      @media screen and (max-width: 1200px) {
        .column {
          width: 100%;
          display: block;
        }    
      } 
      @media screen and (max-width: 1000px) {
        .form-inline label {
          font-size: 15px;
        }   
      } 
      @media (max-width: 900px) {
        .form-inline input.input-text {
          width: 100%;
        }
        .form-inline label {
          /*display: middle;*/
          text-align: center;
          width: 100%;
          
        }        
        .form-inline {
          margin: 0 0 15px;
          flex-direction: column;
          align-items: stretch;
        }
      }      
      @media screen and (max-width: 800px) {
        .column {
          font-size: 15px;
        }    
      }       

      @media screen and (max-width: 700px) {
        .sidenav {
          width: 100%;
          height: auto;
          position: relative;
        }
        /*.sidenav a {float: left;}*/
        div.content {margin-left: 0;}
      }
      @media screen and (max-width: 600px) {
        .column {
          font-size: 14px;
          width: 100%;
          display: block;
        }
      }
      @media screen and (max-width: 400px) {
        .sidenav a {
          text-align: center;
          float: none;
        }
      }




      
      )=====";
#endif
