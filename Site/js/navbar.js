function buildNavBar(page, linkToSelf) {
    var pages = [
        { name:"Home", url: "index.html", },
        { name:"Blog", url: "blog.html", },
        { name:"Media", url: "media.html", },
        { name:"Press", url: "press/index.php", },
    ];

    document.write(`
        <nav>
            <ul class="nav masthead-nav">
    `);

    for (var pageIdx in pages) {
        var navPage = pages[pageIdx];
        document.write("<li");
        if (navPage.name === page) {
            document.write(" class=\"active\"");
        }
        document.write("><a href=\"");
        if (navPage.name !== page || linkToSelf) {
            document.write(navPage.url);
        } else {
            document.write("#");
        }
        document.write("\">" + navPage.name + "</a></li>");
    }

    document.write(`
            </ul>
        </nav>
    `);
}
