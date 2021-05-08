# book-suggestion-system
Using Collaborative Filtering and Pearson Coefficient

The system consists of 3 subsections:
1. Calculating the similarity of a reader to other readers The similarity of a reader to other readers will be calculated using the pearson coefficient. While calculating, action will be taken for the books read by both people. Similarity of persons a and b is calculated using the Pearson coefficient.
2. Determining the k people with whom a reader is the most similar: Based on the result of the previous calculation for a given value of k, find the reader that is most similar to the reader. Plan a more effective solution than listing all the elements to find the k most similar people.
3. Suggesting a new book to the reader: To recommend a book to the reader, calculate with the following equation for the k most similar people. In this equation, the N value indicates the k people the user is most similar to. The calculated pred (a, p) value is the estimated value of the point a person will give to a book that he has not read. 
