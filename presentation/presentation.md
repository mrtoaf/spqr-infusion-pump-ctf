---
marp: true
theme: default
paginate: true
header: '![width:200px](./images/northeastern.png)'
footer: '![width:100px](./images/spqr.png)'
style: |
  section {
    background-color: white;
    color: black;
  }
  h1, h2 {
    color: #000;
  }
  footer {
    text-align: right;
  }
  header {
    text-align: left;
  }
  .columns {
    display: grid;
    grid-template-columns: 1fr 1fr;
    gap: 1rem;
  }
---

# SPQR Open House: The BOMoculars Project

Christopher Pellegrini, Dr. Yan Long, Professor Kevin Fu

---

# The Need for SBOMs

Modern medical devices rely on complex software supply chains with numerous third-party components

- Critical medical devices may contain hundreds of software components
- Many manufacturers lack visibility into their own software
- Software vulnerabilities can impact patient safety

![bg right:40%](./images/image1.jpg)

---

# High Profile Attacks

The 2017 WannaCry ransomware attack and the 2020 SolarWinds breach have accelerated the push towards greater transparency

- Medical devices were significantly impacted by WannaCry, with approximately 34% of UK hospital trusts affected

![bg right:40%](./images/image3.jpg)

---

# Regulatory Landscape

- President Biden issued EO 14028 on Improving the Nation's Cybersecurity
- NTIA responded with Minimum Elements for a Software Bill of Materials in July 2021
- FDA 524(B) requirements for cyber device submissions 
- International Medical Device Regulators Forum (IMDRF) guidance

![bg right:30% vertical](./images/image5.jpg)
![bg right:30% vertical](./images/image4.jpg)
![bg right:30% vertical](./images/image6.jpg)

---

# What is an SBOM?

"An SBOM is like a nutritional label or ingredients list for software"

<div style="font-size: 0.6em;">
<table>
  <tr>
    <th>Data Field</th>
    <th>Description</th>
  </tr>
  <tr>
    <td>Supplier</td>
    <td>Creates/identifies components</td>
  </tr>
  <tr>
    <td>Component</td>
    <td>Software unit by supplier</td>
  </tr>
  <tr>
    <td>Version</td>
    <td>Tracks changes between releases</td>
  </tr>
  <tr>
    <td>Identifiers</td>
    <td>Keys for component lookup</td>
  </tr>
  <tr>
    <td>Dependencies</td>
    <td>Component relationships</td>
  </tr>
  <tr>
    <td>Author</td>
    <td>Creator of SBOM data</td>
  </tr>
  <tr>
    <td>Timestamp</td>
    <td>SBOM creation time</td>
  </tr>
</table>
</div>

![bg right:40% fit](./images/image7.jpg)

---

# The BOMoculars Project

- Evaluating and improving SBOM generation for iOS medical applications
- Project phases: Survey → Benchmark → Tool Development

![bg right:40%](./images/placeholder.png)

---

# Current Status: Survey Phase

- Systematically surveying device manufacturers and healthcare delivery organizations
- Identifying regional differences between domestic and Korean SBOM practices
- Cataloging existing tools and methodologies

![bg right:40%](./images/placeholder.png)

---

# Technical Challenges

- Missing supplier information
- Incomplete dependency relationships
- Unknown versions in Swift packages
- Lack of specialized tools for iOS/Swift ecosystem

![bg right:50%](./images/image12.jpg)

---

# Next Phase: Tool Benchmarking

- Testing existing SBOM generators against SaMD iOS applications
- Metrics: completeness, accuracy, and format compliance
- Focus on open-source medical applications like Loop insulin dosing app

![bg right:40%](./images/placeholder.png)

---

# Building BOMoculars

- Developing a specialized SBOM generation and validation tool for Swift
- Addressing unique challenges of iOS package ecosystem
- Aiming to exceed accuracy rates of existing tools

![bg right:40%](./images/placeholder.png)

---

# Summer Research Opportunities

**Roles Needed:**
- iOS/Swift Developers
- Security Researchers
- Data Visualization Specialists

**Skills You'll Develop:**
- Security analysis
- Medical device regulatory knowledge
- SBOM generation/analysis

![bg right:40%](./images/placeholder.png)

---

# Why This Matters

- Improved SBOMs = Better vulnerability management = Enhanced patient safety
- Our tools will help manufacturers meet regulatory requirements and improve security
- Contributing to the foundation of medical device security

![bg right:40%](./images/placeholder.png)

---

# The "Turtles All The Way Down" Problem

Dependency chains can be endless - where do we stop?

- Standard libraries are tied to runtime versions
- Third-party libraries have their own dependencies
- Finding the right depth is crucial for meaningful SBOMs

![bg right:40%](./images/image19.jpg)

---

# Contact Information

pellegrini.ch@northeastern.edu

Let's chat about summer opportunities!