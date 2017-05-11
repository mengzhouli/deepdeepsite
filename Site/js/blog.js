var blog = {
    entries: [
        {
            title: "First Post",
            author: "Geoff",
            date: new Date("2016-12-26"),
            tags: [ "blog" ],
            entry: "first-post",
        },
    ],
    filterEntries: function(page, elementsPerPage, minDate, maxDate, tag) {
        var result = [];
        for (entryIdx in this.entries) {
            var entry = this.entries[entryIdx];
            if (minDate !== null && entry.date < minDate) {
                continue;
            }
            if (maxDate !== null && entry.date > maxDate) {
                continue;
            }
            if (tag !== null && tag.length > 0 && entry.tags.indexOf(tag) == -1) {
                continue;
            }
            result.push(entry);
        }
        return result.slice(page * elementsPerPage, (page + 1) * elementsPerPage);
    },
    findEntry: function(name) {
        for (entryIdx in this.entries) {
            var entry = this.entries[entryIdx];
            if (entry.entry === name) {
                return entry;
            }
        }
        return null;
    },
    generateEntry: function(element, entryName, linkToSingle) {
        var entry = this.findEntry(entryName);
        if (entry == null) {
            return;
        }

        entryHTML = "";
        entryHTML += "<div class=\"blog-post\">";
        entryHTML += "<h2 class=\"blog-post-title\">";
        if (linkToSingle) {
            entryHTML += "<a href=\"blog-entry.html?entry=" + entryName + "\">";
        }
        entryHTML += entry.title;
        if (linkToSingle) {
            entryHTML += "</a>";
        }
        entryHTML += "</h2>";
        entryHTML += "<p class=\"blog-post-meta\">" + entry.date.toString() +
                     " by <a href=\"team.html?member=" + entry.author + "\">" + entry.author + "</a></p>";
        entryHTML += "<div id=blog-entry-" + entryName + "></div>";

        $("." + element).append(entryHTML);
        $("#blog-entry-" + entryName).load("blog-entries/" + entry.entry + ".html");
    },
    generateEntries: function(element) {
        filteredEntries = this.filterEntries(0, 5, null, null, null);
        for (entryIdx in filteredEntries) {
            this.generateEntry(element, filteredEntries[entryIdx].entry, true);
        }
    },
};
