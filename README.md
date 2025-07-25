# Smart Medical Record Management System (SMRMS)

A comprehensive C-based medical record management system designed to streamline healthcare operations through efficient patient information management, appointment scheduling, prescription management, medicine inventory tracking, medical reporting, user authentication, and emergency patient queue management.

## 📋 Features

### 1. User Authentication & Management
- **Secure Login System**: User authentication with username and password
- **User Management**: Add, view, and manage system users
- **Activity Logging**: Track user activities and system usage
- **Default User Creation**: Automatic setup of default administrator account
- **Role-based Access**: Secure access control for different user levels

### 2. Patient Information Management
- **Patient Registration**: Add new patients with comprehensive information including personal details, medical history, and emergency contacts
- **Advanced Patient Search**: Find patients by ID, name, phone number, or multiple criteria
- **Patient Records**: View, edit, and delete patient information with enhanced data validation
- **Medical History**: Track patient allergies, blood type, and assigned primary doctor
- **Bulk Patient Management**: List all patients and perform batch operations

### 3. Doctor Appointment System
- **Appointment Scheduling**: Schedule appointments with doctors for specific dates and times
- **Appointment Management**: View, edit, and cancel appointments with improved interface
- **Status Tracking**: Monitor appointment status (Scheduled, Completed, Cancelled)
- **Purpose Documentation**: Record the reason for each appointment
- **Appointment History**: Track complete appointment history for patients

### 4. Prescription Management System
- **Digital Prescriptions**: Create and manage electronic prescriptions
- **Medicine Integration**: Link prescriptions with medicine inventory
- **Dosage & Duration**: Specify detailed dosage instructions and treatment duration
- **Prescription Tracking**: View prescriptions by patient or view all prescriptions
- **Billing Integration**: Calculate prescription costs with unit pricing
- **Prescription History**: Maintain complete prescription records with dates and prescribing doctor

### 5. Medicine Inventory Management
- **Comprehensive Medicine Database**: Maintain detailed medicine information including category, manufacturer, and expiry dates
- **Stock Management**: Track medicine quantities and update stock levels with real-time updates
- **Low Stock Alerts**: Identify medicines with low inventory levels
- **Price Management**: Monitor medicine prices and costs with automated calculations
- **Medicine Search**: Advanced search capabilities for quick medicine lookup

### 6. Medical Reports & Analytics
- **Patient Profile Reports**: Generate comprehensive patient information reports
- **Appointment History Reports**: Track patient appointment patterns and history
- **Prescription Reports**: Generate detailed prescription and medication reports
- **Daily Patient Reports**: View daily patient statistics and summaries
- **Custom Report Generation**: Create various types of medical reports with flexible parameters
- **System Usage Reports**: Track system usage and activity patterns

### 7. Emergency Patient Queue Management
- **Priority-Based Queue**: Manage emergency patients with priority levels (Critical, High, Medium, Low)
- **Real-time Status Tracking**: Monitor patient status from arrival to discharge
- **Treatment Documentation**: Record symptoms, treatments, and medications administered
- **Emergency Medicine Management**: Track medicines used in emergency situations
- **Queue Management**: Efficient handling of emergency patient flow

## 🛠️ Technology Stack

- **Language**: C (C17 Standard)
- **Build System**: CMake (version 3.10+)
- **IDE**: CLion (based on project structure)
- **Platform**: Windows (with cross-platform compatibility)
- **Data Storage**: CSV file-based persistent storage
- **Architecture**: Modular design with separated concerns

## 📁 Project Structure

```
├── CMakeLists.txt          # Build configuration
├── README.md               # Project documentation
├── smrms.exe              # Compiled executable
├── include/                # Header files
│   ├── patient.h          # Patient management functions
│   ├── appointment.h      # Appointment system functions
│   ├── medicine.h         # Medicine inventory functions
│   ├── report.h           # Report generation functions
│   ├── emergency.h        # Emergency queue functions
│   ├── auth.h             # Authentication system functions
│   └── prescription.h     # Prescription management functions
├── src/                   # Source files
│   ├── main.c            # Main program entry point
│   ├── patient.c         # Patient management implementation
│   ├── appointment.c     # Appointment system implementation
│   ├── medicine.c        # Medicine inventory implementation
│   ├── report.c          # Report generation implementation
│   ├── emergency.c       # Emergency queue implementation
│   ├── auth.c            # Authentication system implementation
│   └── prescription.c    # Prescription management implementation
├── data/                  # Data storage directory
│   ├── patient.csv       # Patient information storage
│   ├── appointment.csv   # Appointment records
│   ├── medicine.csv      # Medicine inventory data
│   ├── prescription.csv  # Prescription records
│   ├── emergency.csv     # Emergency patient queue
│   ├── reports.csv       # Generated reports
│   ├── users.csv         # User authentication data
│   └── activity.log      # System activity logs
└── cmake-build-debug/     # Build output directory
```

## 🚀 Installation & Setup

### Prerequisites
- C compiler (GCC, Clang, or MSVC)
- CMake (version 3.10 or higher)
- Windows OS (current configuration, adaptable to other platforms)

### Build Instructions

1. **Clone or download the project**
   ```bash
   git clone <repository-url>
   cd SMRMS
   ```

2. **Create build directory**
   ```bash
   mkdir build
   cd build
   ```

3. **Generate build files**
   ```bash
   cmake ..
   ```

4. **Compile the project**
   ```bash
   cmake --build .
   ```

5. **Run the executable**
   ```bash
   ./smrms.exe
   ```

## 💻 Usage

### System Login
- System starts with authentication screen
- Default administrator account is created automatically
- Enter valid credentials to access the main system

### Main Menu Options

Upon successful login, you'll see the main menu with the following options:

1. **Patient Information Lookup** - Comprehensive patient record management
2. **Doctor Appointment** - Complete appointment scheduling and management
3. **Prescription Management** - Digital prescription creation and tracking
4. **Medical Reports** - Advanced report generation and analytics
5. **Emergency Patient Queue** - Priority-based emergency patient handling
6. **User Management** - User authentication and system administration
7. **Medicine Inventory** - Complete medicine stock management
8. **Exit** - Secure system logout

### Navigation
- Use numeric keys (1-8) to select menu options
- Follow intuitive on-screen prompts for specific operations
- Input validation ensures data integrity
- Press Enter to continue after viewing information

## 📊 Data Structures & Models

### Patient Record
- Patient ID (auto-generated), Name, Age, Gender
- Contact information (Phone, Email, Address)
- Medical information (Blood Type, Allergies)
- Emergency contact and primary doctor assignment

### Appointment Record
- Appointment ID (auto-generated), Patient ID, Doctor Name
- Date and Time scheduling with validation
- Purpose documentation and Status tracking
- Comprehensive appointment history

### Prescription Record
- Prescription ID (auto-generated), Patient ID, Medicine ID
- Medicine name, quantity, unit price, total price
- Prescribed date, prescribing doctor
- Dosage instructions, duration, and notes

### Medicine Record
- Medicine ID (auto-generated), Name, Category
- Quantity tracking, Price management, Expiry Date
- Manufacturer information and Description
- Stock level monitoring

### Emergency Patient Record
- Priority-based queue system (Critical, High, Medium, Low)
- Patient symptoms and treatment documentation
- Medicine administration tracking
- Complete arrival to discharge timeline

### User Authentication
- Username and password management
- Activity logging and tracking
- User role management capabilities

## 🔧 Development Features

### Key Technical Implementations
- **Memory Management**: Efficient dynamic memory allocation and deallocation
- **File I/O Operations**: Robust CSV-based data persistence
- **Input Validation**: Comprehensive data validation and error handling
- **Queue Management**: Priority-based emergency patient handling algorithms
- **Search Algorithms**: Multiple search criteria with optimized performance
- **Report Generation**: Automated medical report creation with customizable parameters
- **User Authentication**: Secure login system with activity logging

### Code Quality & Organization
- **Modular Design**: Each functionality separated into dedicated modules
- **Header Files**: Clean separation of interface and implementation
- **Error Handling**: Comprehensive input validation and error management
- **User Interface**: Intuitive console-based menu system
- **Documentation**: Well-documented code with clear function descriptions

## 🆕 Recent Improvements

### Version 2.0 Features
- **Enhanced Security**: Complete user authentication system
- **Prescription Management**: Digital prescription creation and tracking
- **Improved UI**: Better user interface with input validation
- **Data Persistence**: Robust CSV-based data storage
- **Activity Logging**: Complete system activity tracking
- **Advanced Search**: Multi-criteria search capabilities
- **Error Handling**: Comprehensive error management and recovery

## 📈 Future Enhancements

### Planned Features
- **Database Integration**: SQLite/MySQL database support
- **GUI Implementation**: Modern graphical user interface
- **Network Connectivity**: Multi-user access with client-server architecture
- **Advanced Analytics**: Statistical analysis and reporting
- **Mobile Application**: Companion mobile app for healthcare providers
- **Backup & Recovery**: Automated backup and restore functionality
- **Integration APIs**: RESTful API for third-party integrations

### Performance Optimizations
- **Indexing System**: Faster search and retrieval operations
- **Caching Mechanism**: Improved system performance
- **Concurrent Operations**: Multi-threading support
- **Memory Optimization**: Enhanced memory usage efficiency

## 🔒 Security & Compliance

### Security Features
- **User Authentication**: Secure login system
- **Data Validation**: Input sanitization and validation
- **Activity Logging**: Comprehensive audit trail
- **Access Control**: Role-based system access

### Healthcare Compliance
- **Data Privacy**: Patient information protection
- **Audit Trails**: Complete system activity logging
- **Secure Storage**: Encrypted data storage capabilities
- **Regulatory Compliance**: HIPAA-ready architecture

## 🤝 Contributing

This project is designed as a comprehensive medical management system. Contributions for improvements, bug fixes, and feature enhancements are welcome.

### Development Guidelines
- Follow C coding standards and best practices
- Maintain modular code structure
- Include comprehensive error handling
- Document all new features and functions
- Test thoroughly before submitting changes

## 📄 License

This project is developed for educational and professional purposes. Please ensure compliance with healthcare data protection regulations when using in production environments.

## 📞 Support

For technical support, feature requests, or questions about the implementation, please refer to the source code documentation or contact the development team.

---

**Smart Medical Record Management System v2.0** - Advanced healthcare operations management through secure, efficient digital record management with comprehensive prescription and user management capabilities.
