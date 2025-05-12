# Reverse Proxy Server

This repository demonstrates how to implement a **reverse proxy server** with **load balancing**, **concurrency**, and **communication with backend servers** using **C**.

## Folder Structure

```
├── Base.c                # Base code that lays the foundation for the reverse proxy
├── Chapters.txt          # Explanation of the various concepts covered in each chapter
├── fork_proxy.c          # Reverse proxy with process-based concurrency using `fork()`
├── proxy_load_balancer.c # Reverse proxy with round-robin load balancing
├── README.md             # Documentation
├── Reverse_Proxy.c       # Basic reverse proxy server implementation (no load balancing or threading)
├── threaded_proxy.c      # Reverse proxy with threading for concurrent request handling
└── Working_Mechanism.txt # Detailed description of the working mechanism of the reverse proxy
```

## Concepts Covered

### 1. **Reverse Proxy Basics**

* A **reverse proxy** acts as an intermediary between the client and backend servers. Instead of directly contacting backend servers, clients send requests to the reverse proxy, which forwards them to one or more backend servers and returns the response to the client.

### 2. **Load Balancing**

* The reverse proxy balances the load across backend servers. This implementation uses **round-robin load balancing** to cycle through backend servers and ensure an even distribution of incoming requests.

### 3. **Concurrency and Multithreading**

* The reverse proxy is capable of handling multiple client connections concurrently using both **forking** and **multithreading**.

  * `fork_proxy.c` demonstrates the process-based concurrency approach using `fork()`.
  * `threaded_proxy.c` demonstrates the thread-based concurrency approach using `pthread`.

### 4. **Backend Communication Using Sockets**

* The reverse proxy communicates with backend servers via **TCP sockets**. After receiving requests from clients, the proxy forwards them to the appropriate backend server, retrieves the response, and sends it back to the client.

### 5. **Round-Robin Load Balancing**

* Requests are distributed across backend servers using a round-robin algorithm. This ensures that the servers share the traffic load evenly. In the C implementation, backend servers are selected in a cyclic order.

---

## Key Files

### 1. **Base.c**

* Contains the foundation code for the reverse proxy implementation. This file sets up the socket communication and basic flow, serving as a starting point for more complex implementations.

### 2. **fork\_proxy.c**

* This implementation uses **forking** for concurrency, where each incoming client connection spawns a new process to handle the request. This allows the proxy to handle multiple client requests simultaneously, using processes rather than threads.

### 3. **proxy\_load\_balancer.c**

* Implements a reverse proxy with **round-robin load balancing**. This version cycles through multiple backend servers to evenly distribute incoming client requests.

### 4. **Reverse\_Proxy.c**

* A simple reverse proxy implementation without load balancing or multithreading. It handles one client request at a time, forwarding it to a backend server and sending the response back to the client.

### 5. **threaded\_proxy.c**

* This version uses **multithreading** to handle concurrent client requests. Each request is handled by a separate thread, ensuring the proxy can manage multiple connections concurrently without blocking.

### 6. **Working\_Mechanism.txt**

* Provides a detailed explanation of the working mechanism of the reverse proxy server, including how it handles incoming client requests, communicates with backend servers, and implements load balancing and concurrency.

### 7. **Chapters.txt**

* Describes the various concepts and stages covered in the project, including the development and evolution of the reverse proxy server, load balancing strategies, and concurrency models.

---

## How to Run

### 1. **Compile and Run the Reverse Proxy**

To get started, you'll need to compile and run the reverse proxy server. Follow these steps:

```bash
# In the main project directory
gcc -o reverse_proxy Reverse_Proxy.c  # Compile the basic reverse proxy
./reverse_proxy                       # Run the reverse proxy on port 8080
```

### 2. **Test the Proxy**

Use an HTTP client like `curl` to test the reverse proxy. For example:

```bash
curl http://localhost:8080/api/data
```

You should receive a response from one of the backend servers, depending on the load balancing strategy (round-robin).

---

## Key Concepts

### TCP Sockets

* The proxy server communicates with backend servers using **TCP sockets**, a low-level mechanism for network communication. Each backend server listens on a different port (e.g., 8000, 8001, 8002), and the proxy forwards the client request to the appropriate server based on the load balancing strategy.

### Forking vs. Threading

* **Forking** (as seen in `fork_proxy.c`) creates a new process for each client request. This approach can be easier to implement but might be less efficient due to the overhead of creating new processes.
* **Threading** (as seen in `threaded_proxy.c`) creates a new thread for each client request, which tends to be more efficient in terms of resource usage compared to forking.

### Round-Robin Load Balancing

* The reverse proxy uses the **round-robin load balancing** technique, distributing client requests evenly across available backend servers. The proxy keeps track of the current backend server and cycles through the servers as requests come in.

---

## Future Enhancements

1. **Express Backend Integration**: Implement Express.js backend servers to test the reverse proxy with real-world backend APIs.
2. **Health Checks**: Add health checks to ensure that the proxy only forwards requests to healthy backend servers.
3. **Timeouts and Retries**: Implement timeout handling and retries for failed backend connections.
4. **Enhanced Load Balancing**: Implement more sophisticated load balancing algorithms (e.g., weighted round-robin, least connections, etc.).

---

## Conclusion

This project demonstrates the implementation of a reverse proxy server with basic load balancing and concurrency in C. The proxy handles client requests, balances the load across backend servers, and communicates with backend APIs using TCP sockets. The forking and threading approaches provide flexibility in how the proxy handles concurrent client requests, with each method offering different performance characteristics.

