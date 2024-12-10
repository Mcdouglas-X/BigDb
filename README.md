
**BigDb** is an example of how to exponentially increase a database of sequentially ordered public keys.


A database of sequential points is created and stored in a Bloom filter.


## Creating the Database


**To create a database with 1,000,000 points:**


```bash
./BigDb -bs 1000000
```


Once created, a log file is generated that records the point count. In this case, the log file will record **1,000,000** points. This log file serves as a guide both for expanding the database and for performing accurate searches.

## Example of Expanding the Database

**If we want to add 1,000,000 more points:**

```bash
./BigDb -bs 1000000
```
This will start creating points from where the log file indicates. For our example, points will be created from **1,000,000** to **2,000,000** (because we are adding **1,000,000** points).

## Using the Database
**Once our database is created, we run:**

```bash
./BigDb -f 30.txt -ht 1000
```
The ```-f``` parameter will read from the "30.txt" file the search range and the target public key, and will create a hash table with the ```-ht``` parameter.

**The hash table is composed as follows:**

It reads the log file to know the size of our database.

It subtracts this amount (**2,000,000** in the example) from the target the number of times indicated by ```-ht``` (**1,000** in the example).

This subtracts **2,000,000** from the public key **1,000** times consecutively, temporarily storing all intermediate results in the hash table.


We have increased the range covered by the database by **1,000** times to **2,000,000** * **1,000** = **2,000,000,000**.

This improvement requires extra computation power for the search (explained later).


## Search


**For the search in this example, we use traditional brute force:**

A random number is created within the search range.

This random point is searched by finding the difference with each element of the hash table by subtracting.

If the random point is within the range "target - 2,000,000,000:target", eventually one of those results will be in the Bloom filter and we can easily calculate the private key.

Although we could exponentially increase our database (**x10, x100, x1000... x1,000,000**), the computational load also increases. In our example, it would add **1,000** additional subtractions.



Thanks for reading. This example is not intended for actual use; the code is just a poorly optimized example.



**Donate to:**


**btc: bc1qxs47ttydl8tmdv8vtygp7dy76lvayz3r6rdahu**
