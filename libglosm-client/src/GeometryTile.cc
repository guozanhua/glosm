/*
 * Copyright (C) 2010-2011 Dmitry Marakasov
 *
 * This file is part of glosm.
 *
 * glosm is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * glosm is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with glosm.  If not, see <http://www.gnu.org/licenses/>.
 */

#define GL_GLEXT_PROTOTYPES

#if defined(__APPLE__)
#	include <OpenGL/gl.h>
#else
#	include <GL/gl.h>
#endif

#include <glosm/GeometryTile.hh>

#include <glosm/Projection.hh>
#include <glosm/Geometry.hh>
#include <glosm/GeometryDatasource.hh>
#include <glosm/SimpleVertexBuffer.hh>

GeometryTile::GeometryTile(const Projection& projection, const Geometry& geom, const Vector2i& ref, const BBoxi& bbox = BBoxi::ForEarth()) : Tile(ref) {
	if (geom.GetLines().size() != 0) {
		std::vector<Vector3f> translated;
		projection.ProjectPoints(geom.GetLines(), ref, translated);
		lines_.reset(new SimpleVertexBuffer(SimpleVertexBuffer::LINES, translated.data(), translated.size()));
	}

	if (geom.GetTriangles().size() != 0) {
		std::vector<Vector3f> translated;
		projection.ProjectPoints(geom.GetTriangles(), ref, translated);
		triangles_.reset(new SimpleVertexBuffer(SimpleVertexBuffer::TRIANGLES, translated.data(), translated.size()));
	}

	if (geom.GetQuads().size() != 0) {
		std::vector<Vector3f> translated;
		projection.ProjectPoints(geom.GetQuads(), ref, translated);
		quads_.reset(new SimpleVertexBuffer(SimpleVertexBuffer::QUADS, translated.data(), translated.size()));
	}

#ifdef TILE_DEBUG
	bound_[0] = projection.Project(bbox.GetTopLeft(), ref);
	bound_[1] = projection.Project(bbox.GetTopRight(), ref);
	bound_[2] = projection.Project(bbox.GetBottomRight(), ref);
	bound_[3] = projection.Project(bbox.GetBottomLeft(), ref);
#endif
}

GeometryTile::~GeometryTile() {
}

void GeometryTile::Render() const {
	if (lines_.get()) {
		glDepthFunc(GL_LESS);

		glColor4f(0.0f, 0.0f, 0.0f, 0.5f);

		/*glEnable(GL_LINE_STIPPLE);
		glLineStipple(1.0, 0xCCCC);
		glDisable(GL_LINE_STIPPLE);
		*/

		lines_->Render();
	}

	if (triangles_.get() || quads_.get()) {
		/* polygons */
		glPolygonOffset(1.0, 1.0);
		glEnable(GL_POLYGON_OFFSET_FILL);

		/* zpass */
		/*glColor4f(0.0f, 0.0f, 0.0f, 0.0f);
		triangles_->Render();
		quads_->Render();

		glDepthFunc(GL_EQUAL);*/

		/* objects */
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);

		if (triangles_.get())
			triangles_->Render();
		if (quads_.get())
			quads_->Render();

		glDisable(GL_LIGHT0);
		glDisable(GL_LIGHTING);

		glDisable(GL_POLYGON_OFFSET_FILL);
	}

#ifdef TILE_DEBUG
	glColor3f(1.0, 0.0, 0.0);
	glBegin(GL_LINE_LOOP);
	glVertex3f(bound_[0].x, bound_[0].y, 0.0);
	glVertex3f(bound_[1].x, bound_[1].y, 0.0);
	glVertex3f(bound_[2].x, bound_[2].y, 0.0);
	glVertex3f(bound_[3].x, bound_[3].y, 0.0);
	glEnd();
#endif
}
