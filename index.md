---
layout: default
title: Welcome to Rust Lessons
---

# Welcome to Rust Lessons

## 🚀 Latest Lessons
{% for post in site.posts %}
- [{{ post.title }}]({{ post.url | relative_url }}) - *{{ post.date | date: "%B %d, %Y" }}* <!-- Changed permalink → url -->
{% endfor %}


---

## 🎯 What You'll Learn
Gain a solid foundation and sharpen your Rust skills:

- 🚩 Rust fundamentals
- 🛠️ Memory management and ownership
- 📊 Working with data types, loops, and functions
- ⚡ Advanced topics: concurrency and error handling
- 💻 Real-world examples and best practices

---

## 🌟 Why Rust?
Rust is fast, reliable, and loved by developers worldwide. It combines the performance of systems-level languages with the safety and modern features you’d expect. Whether you're building web servers, embedded systems, or CLI tools, Rust has got you covered.

---

## 💡 Start Learning Now!
Click on any lesson above to get started, or explore the site using the navigation links. Happy coding!
