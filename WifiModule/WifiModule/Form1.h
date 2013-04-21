#pragma once

namespace WifiModule {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace System::Net;
	using namespace System::Net::Sockets;
	using namespace System::Net::NetworkInformation;
	using namespace System::IO;
	using namespace System::Text;
	using namespace System::Threading;

	/// <summary>
	/// Summary for Form1
	/// </summary>
	public ref class Form1 : public System::Windows::Forms::Form
	{
	public:
		/****The csv file to send****/
		String^ file;

		/****The LED module to send to, identified by IP address****/
		String^ LEDmodule;
		String^ Keyword;

		/****Holds the list of modules active (IP addresses) and the file(s) selected to load to them****/
		array<String^>^ IPadd;
		array<String^>^ Files;

	private: System::Windows::Forms::TextBox^  fileSelectedTextBox;

	public: 

		/****Flag to see if listbox has been clicked****/
		int listboxclick;
	private: System::ComponentModel::BackgroundWorker^  backgroundWorkerRefresh;


	public: 

		/****Converts file to send to LED module to byte array****/
		array<Byte> ^FileToByteArray(String ^_FileName)
		{
			array<Byte> ^_Buffer = nullptr;

			try
			{
				// Open file for reading
				System::IO::FileStream ^_FileStream = gcnew System::IO::FileStream(_FileName, System::IO::FileMode::Open, System::IO::FileAccess::Read);

				// attach filestream to binary reader
				System::IO::BinaryReader ^_BinaryReader = gcnew System::IO::BinaryReader(_FileStream);

				// get total byte length of the file
				System::IO::FileInfo ^_FileInfo = gcnew System::IO::FileInfo(_FileName);

				System::Int64 _TotalBytes = _FileInfo->Length;

				// read entire file into buffer
				_Buffer = _BinaryReader->ReadBytes(safe_cast<Int32>(_TotalBytes));

				// close file reader
				_FileStream->Close();
				delete _FileStream;
				_BinaryReader->Close();
			}
			catch (Exception ^_Exception)
			{
				// Error
				Console::WriteLine("Exception caught in process: {0}", _Exception->ToString());
			}
			return _Buffer;
		}

		/****Sends file to the selected LED module****/
		void Connect( String^ LEDmodule, array<Byte>^ data )
		{
			try
			{
				// Create a TcpClient. 
				// Note, for this client to work you need to have a TcpServer  
				// connected to the same address as specified by the server, port 
				// combination.
				Int32 port = 2000;
				TcpClient^ client = gcnew TcpClient(LEDmodule, port);

				NetworkStream^ stream = client->GetStream();

				// Send the message to the connected TcpServer. 
				stream->Write( data, 0, data->Length );

				// Close everything.
				client->Close();
			}
			catch ( ArgumentNullException^ e ) 
			{
				MessageBox::Show("Could not connect, ArgumentNullException"); 
				//Console::WriteLine( "ArgumentNullException: {0}", e );
			}
			catch ( SocketException^ e ) 
			{
				MessageBox::Show("Could not connect, SocketException");
				//Console::WriteLine( "SocketException: {0}", e );
			}
		}

		void Listen()
		{
			try
			{
				// Set the TcpListener on port 4000.
				Int32 port = 4000;
				IPAddress^ localAddr = IPAddress::Parse("192.168.1.149");//("170.255.1.0");

				// TcpListener* server = new TcpListener(port);
				TcpListener^ server = gcnew TcpListener( localAddr,port );

				// Start listening for client requests.
				server->Start();

				// Buffer for reading data 
				array<Byte>^bytes = gcnew array<Byte>(256);
				String^ data = nullptr;

				// Enter the listening loop. 
				while ( true )
				{
					// Perform a blocking call to accept requests. 
			
					TcpClient^ client = server->AcceptTcpClient();
			
					data = nullptr;

					// Get a stream Object* for reading and writing
					NetworkStream^ stream = client->GetStream();
					Int32 i;

					// Loop to receive all the data sent by the client. 
					while ( i = stream->Read( bytes, 0, bytes->Length ) )
					{

						Encoding^ ascii = Encoding::ASCII;

						// Translate data bytes to a ASCII String
						data = ascii->GetString( bytes );
						
						if (data == "D")
						{
							client->Close();
							return;
						}
					}

					// Shutdown and end connection
					client->Close();
				}
			}
			catch ( SocketException^ e ) 
			{
				Console::WriteLine( "SocketException: {0}", e );
			}
		}

		Form1(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//

			try 
			{
				/****Pings a list if IP addresses to fill listbox with all active modules****/
				String^ left = "192.168.1.";//"170.255.1.";
				int rightInt = 109;//1;
				String^ right;
				for (int i = 0; i < 5; i++)
				{
					right = System::Convert::ToString(rightInt);
					String^ ipAddress = left + right;
					Ping ^ pingSender = gcnew Ping;
					IPAddress^ address = IPAddress::Parse(ipAddress);
					PingReply^ reply = pingSender->Send(address);

					if ( reply->Status == IPStatus::Success )
					{
						wifiListBox->Items->Add(ipAddress);
					}

					rightInt++;
				}
			}
			catch (PingException^ e)
			{
				MessageBox::Show("Error in locating LED modules, please press the refresh button");
			}
			/****Number of modules in listbox****/
			int n = wifiListBox->Items->Count;
			
			/****IPadd is an array containing all the modules in the listbox****/
			IPadd = gcnew array<String^>(n);

			/****Files is an array containing the filenames to be loaded onto the corresponding modules in IPadd with the same indices****/
			/****If no file opened for the module, then value is NULL****/
			Files = gcnew array<String^>(n);

			for (int i = 0; i < n; i++)
			{
				IPadd[i] = wifiListBox->Items[i]->ToString();
				Files[i] = "Please Select a File";
			}

		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~Form1()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::ListBox^  wifiListBox;
	protected: 
	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::Button^  refreshButton;
	private: System::Windows::Forms::Button^  playButton;
	private: System::Windows::Forms::Button^  stopButton;
	private: System::Windows::Forms::Button^  loadButton;
	private: System::Windows::Forms::Button^  chooseButton;

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(Form1::typeid));
			this->wifiListBox = (gcnew System::Windows::Forms::ListBox());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->refreshButton = (gcnew System::Windows::Forms::Button());
			this->playButton = (gcnew System::Windows::Forms::Button());
			this->stopButton = (gcnew System::Windows::Forms::Button());
			this->loadButton = (gcnew System::Windows::Forms::Button());
			this->chooseButton = (gcnew System::Windows::Forms::Button());
			this->fileSelectedTextBox = (gcnew System::Windows::Forms::TextBox());
			this->backgroundWorkerRefresh = (gcnew System::ComponentModel::BackgroundWorker());
			this->SuspendLayout();
			// 
			// wifiListBox
			// 
			this->wifiListBox->FormattingEnabled = true;
			this->wifiListBox->Location = System::Drawing::Point(25, 34);
			this->wifiListBox->Margin = System::Windows::Forms::Padding(2);
			this->wifiListBox->Name = L"wifiListBox";
			this->wifiListBox->Size = System::Drawing::Size(112, 108);
			this->wifiListBox->TabIndex = 0;
			this->wifiListBox->SelectedIndexChanged += gcnew System::EventHandler(this, &Form1::wifiListBox_SelectedIndexChanged);
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(22, 15);
			this->label1->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(71, 13);
			this->label1->TabIndex = 1;
			this->label1->Text = L"LED Modules";
			// 
			// refreshButton
			// 
			this->refreshButton->Location = System::Drawing::Point(25, 145);
			this->refreshButton->Margin = System::Windows::Forms::Padding(2);
			this->refreshButton->Name = L"refreshButton";
			this->refreshButton->Size = System::Drawing::Size(112, 20);
			this->refreshButton->TabIndex = 2;
			this->refreshButton->Text = L"Refresh";
			this->refreshButton->UseVisualStyleBackColor = true;
			this->refreshButton->Click += gcnew System::EventHandler(this, &Form1::refreshButton_Click);
			// 
			// playButton
			// 
			this->playButton->Location = System::Drawing::Point(22, 223);
			this->playButton->Margin = System::Windows::Forms::Padding(2);
			this->playButton->Name = L"playButton";
			this->playButton->Size = System::Drawing::Size(72, 45);
			this->playButton->TabIndex = 3;
			this->playButton->Text = L"Play";
			this->playButton->UseVisualStyleBackColor = true;
			this->playButton->Click += gcnew System::EventHandler(this, &Form1::playButton_Click);
			// 
			// stopButton
			// 
			this->stopButton->Location = System::Drawing::Point(236, 223);
			this->stopButton->Margin = System::Windows::Forms::Padding(2);
			this->stopButton->Name = L"stopButton";
			this->stopButton->Size = System::Drawing::Size(72, 45);
			this->stopButton->TabIndex = 4;
			this->stopButton->Text = L"Stop";
			this->stopButton->UseVisualStyleBackColor = true;
			this->stopButton->Click += gcnew System::EventHandler(this, &Form1::stopButton_Click);
			// 
			// loadButton
			// 
			this->loadButton->Location = System::Drawing::Point(236, 122);
			this->loadButton->Margin = System::Windows::Forms::Padding(2);
			this->loadButton->Name = L"loadButton";
			this->loadButton->Size = System::Drawing::Size(72, 20);
			this->loadButton->TabIndex = 5;
			this->loadButton->Text = L"Load Files";
			this->loadButton->UseVisualStyleBackColor = true;
			this->loadButton->Click += gcnew System::EventHandler(this, &Form1::loadButton_Click);
			// 
			// chooseButton
			// 
			this->chooseButton->Location = System::Drawing::Point(236, 34);
			this->chooseButton->Margin = System::Windows::Forms::Padding(2);
			this->chooseButton->Name = L"chooseButton";
			this->chooseButton->Size = System::Drawing::Size(72, 20);
			this->chooseButton->TabIndex = 6;
			this->chooseButton->Text = L"Choose File";
			this->chooseButton->UseVisualStyleBackColor = true;
			this->chooseButton->Click += gcnew System::EventHandler(this, &Form1::chooseButton_Click);
			// 
			// fileSelectedTextBox
			// 
			this->fileSelectedTextBox->Location = System::Drawing::Point(142, 59);
			this->fileSelectedTextBox->Name = L"fileSelectedTextBox";
			this->fileSelectedTextBox->ReadOnly = true;
			this->fileSelectedTextBox->Size = System::Drawing::Size(168, 20);
			this->fileSelectedTextBox->TabIndex = 8;
			this->fileSelectedTextBox->Text = L"Please select a Module";
			// 
			// backgroundWorkerRefresh
			// 
			this->backgroundWorkerRefresh->RunWorkerCompleted += gcnew System::ComponentModel::RunWorkerCompletedEventHandler(this, &Form1::backgroundWorkerRefresh_RunWorkerCompleted);
			// 
			// Form1
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(340, 293);
			this->Controls->Add(this->fileSelectedTextBox);
			this->Controls->Add(this->chooseButton);
			this->Controls->Add(this->loadButton);
			this->Controls->Add(this->stopButton);
			this->Controls->Add(this->playButton);
			this->Controls->Add(this->refreshButton);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->wifiListBox);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedSingle;
			this->Icon = (cli::safe_cast<System::Drawing::Icon^  >(resources->GetObject(L"$this.Icon")));
			this->Margin = System::Windows::Forms::Padding(2);
			this->MaximizeBox = false;
			this->Name = L"Form1";
			this->Text = L"LED GUI";
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	private: System::Void chooseButton_Click(System::Object^  sender, System::EventArgs^  e) {
				 

				 if (listboxclick == 1)
				 {
					 // Displays an OpenFileDialog so the user can select a Cursor.
					 OpenFileDialog ^ openFileDialog1 = gcnew OpenFileDialog();
					 openFileDialog1->Filter = "LED Files|*.csv";
					 openFileDialog1->Title = "Select an LED File";

					 // Show the Dialog.
					 // If the user clicked OK in the dialog and a .csv file was selected, open it.
					 if(openFileDialog1->ShowDialog() == System::Windows::Forms::DialogResult::OK)
					 {
						 file = openFileDialog1->FileName;
						 fileSelectedTextBox->Text = file;
					 }
					 
					 for (int i = 0; i < Files->Length; i++)
					 {
						if (wifiListBox->SelectedItem->ToString() == IPadd[i])
						{
							Files[i] = file;
						}
					 }
				 }
			 }
private: System::Void loadButton_Click(System::Object^  sender, System::EventArgs^  e) {

			 for (int i = 0; i < Files->Length; i++)
			 {
				 //If a module does not yet have a file loaded
				 if (Files[i] == "Please Select a File")
				 {
					 MessageBox::Show("Please choose files for all modules before loading!");
					 return;
				 }
			 }

			 for (int j = 0; j < Files->Length; j++)
			 {
				 
				 StreamReader^ din = File::OpenText(Files[j]);

				 String^ frames = nullptr;
				 String^ temp = nullptr;
				 array<Byte>^ byte_frames;
				 bool end_of_file = false;
				 int line = 1;

				 while ((temp = din->ReadLine()) != nullptr) 
				 {
					 //send every 2 lines as a string
					 if (line%2 == 0)
					 {
						 frames += temp;
						 Encoding^ ascii = Encoding::ASCII;
						 byte_frames = ascii->GetBytes(frames);
						 Connect(IPadd[j],byte_frames);
						 Listen();
						 frames = nullptr;
						 line++;

						 //wait for ack to be received
						 /*while(recv == 0)
						 {
							 if (recv == 1)
							 {
								 recv = 0;
								 break;
							 }
						 }*/
					 }
					 else 
					 {
						 frames += temp;
						 line++;
					 }
				 }

				 if (frames != nullptr)
				 {
					 Encoding^ ascii = Encoding::ASCII;
					 byte_frames = ascii->GetBytes(frames);
					 Connect(IPadd[j],byte_frames);
				 }
				 MessageBox::Show("File sent!");
			 }
		 }
private: System::Void refreshButton_Click(System::Object^  sender, System::EventArgs^  e) {

			 backgroundWorkerRefresh->RunWorkerAsync();
		 }
private: System::Void wifiListBox_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) {
			 listboxclick = 1;

			 fileSelectedTextBox->Text = Files[wifiListBox->SelectedIndex];
		 }
private: System::Void playButton_Click(System::Object^  sender, System::EventArgs^  e) {

			 Keyword = "P";
			 Encoding^ ascii = Encoding::ASCII;
			 // Translate the passed message into ASCII and store it as a Byte array. 
			 array<Byte>^ message = ascii->GetBytes(Keyword);

			 for (int j = 0; j < Files->Length; j++)
			 {
				 Connect(IPadd[j], message);
			 }
		 }
private: System::Void stopButton_Click(System::Object^  sender, System::EventArgs^  e) {

			 Keyword = "T";
			 Encoding^ ascii = Encoding::ASCII;
			 // Translate the passed message into ASCII and store it as a Byte array. 
			 array<Byte>^ message = ascii->GetBytes(Keyword);

			 for (int j = 0; j < Files->Length; j++)
			 {
				 Connect(IPadd[j], message);
			 }
		 }
private: System::Void backgroundWorkerRefresh_RunWorkerCompleted(System::Object^  sender, System::ComponentModel::RunWorkerCompletedEventArgs^  e) {

			 wifiListBox->Items->Clear();
			 listboxclick = 0;

			 /****Pings a list if IP addresses to fill listbox with all active modules****/
			 String^ left = "192.168.1.";//"170.255.1.";
			 int rightInt = 109;//1;
			 String^ right;
			 for (int i = 0; i < 5; i++)
			 {
				 right = System::Convert::ToString(rightInt);
				 String^ ipAddress = left + right;
				 Ping ^ pingSender = gcnew Ping;
				 IPAddress^ address = IPAddress::Parse(ipAddress);
				 PingReply ^ reply = pingSender->Send(address);

				 if ( reply->Status == IPStatus::Success )
				 {
					 wifiListBox->Items->Add(ipAddress);
				 }

				 rightInt++;
			 }

			 /****Number of modules in listbox****/
			 int n = wifiListBox->Items->Count;

			 for (int i = 0; i < n; i++)
			 {
				 IPadd[i] = wifiListBox->Items[i]->ToString();
				 Files[i] = "Please Select a File";
			 }

			 fileSelectedTextBox->Text = "Please select a Module";

			 //MessageBox::Show("Refreshed!");

		 }
};
}

